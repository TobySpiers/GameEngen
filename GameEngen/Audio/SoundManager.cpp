#define DR_WAV_IMPLEMENTATION
#include <dr_wav.h>

#define DR_MP3_IMPLEMENTATION
#include <dr_mp3.h>

#include "SoundManager.h"

#include "UserSettings/AudioSettings.h"

#include <cstdio>
#include <cstring>

SoundManager::SoundManager(int maxSources)
    : maxSources(maxSources)
{
    device = alcOpenDevice(nullptr);
    if (!device)
    {
        fprintf(stderr, "OpenAL: failed to open device\n");
        return;
    }

    context = alcCreateContext(device, nullptr);
    if (!context)
    {
        fprintf(stderr, "OpenAL: failed to create context\n");
        alcCloseDevice(device);
        device = nullptr;
        return;
    }

    alcMakeContextCurrent(context);
}

SoundManager::~SoundManager()
{
    // Stop and clean up all active music streams
    for (auto& [id, stream] : musicStreams)
    {
        StopMusic(id);
    }

    if (!sources.empty())
        alDeleteSources(static_cast<ALsizei>(sources.size()), sources.data());

    for (auto& [id, buf] : soundBuffers)
        alDeleteBuffers(1, &buf);

    alcMakeContextCurrent(nullptr);
    if (context) alcDestroyContext(context);
    if (device)  alcCloseDevice(device);
}

// =============================================================================
// One-shot sounds
// =============================================================================

SoundId SoundManager::LoadSound(const std::string& path)
{
    // Return the cached id if this file has already been loaded
    if (auto it = pathToId.find(path); it != pathToId.end())
    {
        return it->second;
    }

    unsigned int channels;
    unsigned int sampleRate;
    drwav_uint64 totalFrames;
    drwav_int16* data = drwav_open_file_and_read_pcm_frames_s16(
        path.c_str(), &channels, &sampleRate, &totalFrames, nullptr);

    if (!data)
    {
        fprintf(stderr, "SoundManager: failed to load sound '%s'\n", path.c_str());
        return InvalidSound;
    }

    ALenum format = (channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

    ALuint buffer;
    alGenBuffers(1, &buffer);
    alBufferData(buffer, format, data,
        static_cast<ALsizei>(totalFrames * channels * sizeof(drwav_int16)),
        static_cast<ALsizei>(sampleRate));

    drwav_free(data, nullptr);

    SoundId id        = nextSoundId++;
    soundBuffers[id]  = buffer;
    pathToId[path]    = id;
    return id;
}

void SoundManager::Play(SoundId id)
{
    auto it = soundBuffers.find(id);
    if (it == soundBuffers.end())
        return;

    ALuint source = AcquireSource();
    alSourcei(source, AL_BUFFER, static_cast<ALint>(it->second));
    alSourcef(source, AL_GAIN, AudioSettings::Get().soundEffectsVolume);
    // Source-relative: plays at the listener regardless of world position
    alSourcei(source, AL_SOURCE_RELATIVE, AL_TRUE);
    alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);
    alSourcePlay(source);
}

void SoundManager::PlayAt(SoundId id, glm::vec3 position)
{
    auto it = soundBuffers.find(id);
    if (it == soundBuffers.end())
        return;

    ALuint source = AcquireSource();
    alSourcei(source, AL_BUFFER, static_cast<ALint>(it->second));
    alSourcef(source, AL_GAIN, AudioSettings::Get().soundEffectsVolume);
    alSourcei(source, AL_SOURCE_RELATIVE, AL_FALSE);
    alSource3f(source, AL_POSITION, position.x, position.y, position.z);
    alSourcePlay(source);
}

ALuint SoundManager::AcquireSource()
{
    // Reuse any source that has finished playing
    for (ALuint src : sources)
    {
        ALint state;
        alGetSourcei(src, AL_SOURCE_STATE, &state);
        if (state != AL_PLAYING)
        {
            alSourceStop(src);
            return src;
        }
    }

    // All sources are busy — allocate a new one if below the cap
    if (static_cast<int>(sources.size()) < maxSources)
    {
        ALuint src;
        alGenSources(1, &src);
        sources.push_back(src);
        return src;
    }

    // At the cap — steal the first source (oldest still-playing sound)
    ALuint src = sources.front();
    alSourceStop(src);
    return src;
}

// =============================================================================
// Music streaming
// =============================================================================

MusicId SoundManager::LoadMusic(const std::string& path)
{
    if (auto it = musicPathToId.find(path); it != musicPathToId.end())
    {
        return it->second;
    }

    // Validate the file is openable before committing to an id
    drmp3 probe;
    if (!drmp3_init_file(&probe, path.c_str(), nullptr))
    {
        fprintf(stderr, "SoundManager: failed to open music '%s'\n", path.c_str());
        return InvalidMusic;
    }
    drmp3_uninit(&probe);

    MusicId id              = nextMusicId++;
    musicStreams[id].path   = path;
    musicPathToId[path]     = id;
    return id;
}

void SoundManager::PlayMusic(MusicId id, bool loop)
{
    auto it = musicStreams.find(id);
    if (it == musicStreams.end())
        return;

    MusicStream& stream = it->second;

    // Tear down any previous playback for this id
    if (stream.active)
    {
        StopMusic(id);
    }

    if (!drmp3_init_file(&stream.decoder, stream.path.c_str(), nullptr))
    {
        fprintf(stderr, "SoundManager: failed to open music '%s'\n", stream.path.c_str());
        return;
    }

    alGenSources(1, &stream.source);
    alGenBuffers(NumStreamBuffers, stream.buffers);
    alSourcef(stream.source, AL_GAIN, AudioSettings::Get().musicVolume);
    stream.looping = loop;
    stream.active  = true;

    // Fill all buffers upfront and queue them
    for (int i = 0; i < NumStreamBuffers; ++i)
        FillBuffer(stream, stream.buffers[i]);

    alSourceQueueBuffers(stream.source, NumStreamBuffers, stream.buffers);
    alSourcePlay(stream.source);
}

void SoundManager::StopMusic(MusicId id)
{
    auto it = musicStreams.find(id);
    if (it == musicStreams.end() || !it->second.active)
    {
        return;
    }

    MusicStream& stream = it->second;

    alSourceStop(stream.source);

    // Unqueue any remaining buffers before deleting
    ALint queued = 0;
    alGetSourcei(stream.source, AL_BUFFERS_QUEUED, &queued);
    while (queued--)
    {
        ALuint buf;
        alSourceUnqueueBuffers(stream.source, 1, &buf);
    }

    alDeleteSources(1, &stream.source);
    alDeleteBuffers(NumStreamBuffers, stream.buffers);
    drmp3_uninit(&stream.decoder);

    stream.source = 0;
    memset(stream.buffers, 0, sizeof(stream.buffers));
    stream.active = false;
}

void SoundManager::Update()
{
    for (auto& [id, stream] : musicStreams)
    {
        if (!stream.active)
            continue;

        alSourcef(stream.source, AL_GAIN, AudioSettings::Get().musicVolume);

        ALint processed = 0;
        alGetSourcei(stream.source, AL_BUFFERS_PROCESSED, &processed);

        while (processed--)
        {
            ALuint buf;
            alSourceUnqueueBuffers(stream.source, 1, &buf);

            int frames = FillBuffer(stream, buf);
            if (frames > 0)
            {
                alSourceQueueBuffers(stream.source, 1, &buf);
            }
            else if (stream.looping)
            {
                // Seek back to the start and keep going
                drmp3_seek_to_pcm_frame(&stream.decoder, 0);
                FillBuffer(stream, buf);
                alSourceQueueBuffers(stream.source, 1, &buf);
            }
            // If not looping and no frames, we simply don't re-queue;
            // the source will stop naturally once all queued buffers drain.
        }

        // Guard against buffer underrun: if the source stopped but we're still
        // active (e.g. Update() was called too infrequently), restart it.
        ALint state = 0;
        alGetSourcei(stream.source, AL_SOURCE_STATE, &state);
        ALint queued = 0;
        alGetSourcei(stream.source, AL_BUFFERS_QUEUED, &queued);

        if (state != AL_PLAYING && queued > 0)
        {
            alSourcePlay(stream.source);
        }
    }
}

int SoundManager::FillBuffer(MusicStream& stream, ALuint buffer)
{
    // dr_mp3 always outputs stereo (2 channels) at the file's sample rate
    static constexpr int channels = 2;
    drmp3_int16 pcm[StreamBufferFrames * channels];

    drmp3_uint64 framesDecoded = drmp3_read_pcm_frames_s16(
        &stream.decoder, StreamBufferFrames, pcm);

    if (framesDecoded > 0)
    {
        alBufferData(buffer, AL_FORMAT_STEREO16, pcm,
            static_cast<ALsizei>(framesDecoded * channels * sizeof(drmp3_int16)),
            static_cast<ALsizei>(stream.decoder.sampleRate));
    }

    return static_cast<int>(framesDecoded);
}
