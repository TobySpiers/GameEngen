#pragma once

#include "ServiceLocator.h"
#include "Audio/AudioTypes.h"

#include <string>
#include <unordered_map>
#include <vector>

#include <AL/al.h>
#include <AL/alc.h>
#include <dr_mp3.h>
#include <glm/glm.hpp>

class SoundManager
{
public:
    static SoundManager& Get() { return ServiceLocator::GetSoundManager(); }
public:
    explicit SoundManager(int maxSources = 32);
    ~SoundManager();

    SoundManager(const SoundManager&)            = delete;
    SoundManager& operator=(const SoundManager&) = delete;

    // --- One-shot sounds (WAV, fully decoded into memory) ---

    // Decodes a WAV file into an OpenAL buffer and returns a SoundId.
    // Subsequent calls with the same path return the cached id.
    // Returns InvalidSound if the file cannot be read.
    SoundId LoadSound(const std::string& path);

    // Plays a previously loaded sound as a fire-and-forget one-shot.
    // Source-relative: plays at the listener's position regardless of world position.
    void Play(SoundId id);

    // Plays a previously loaded sound at a specific world-space position.
    // Volume and panning are affected by distance and direction from the listener.
    void PlayAt(SoundId id, glm::vec3 position);

    // --- Music (MP3, streamed from disk) ---

    // Validates and registers a music file path, returning a MusicId.
    // No decoding happens here — streaming begins on PlayMusic().
    // Returns InvalidMusic if the file cannot be opened.
    MusicId LoadMusic(const std::string& path);

    // Opens the decoder, fills the initial streaming buffers, and starts playback.
    void PlayMusic(MusicId id, bool loop = true);

    // Stops and tears down the active stream for this id.
    void StopMusic(MusicId id);

    // Must be called once per frame to refill streaming buffers.
    void Update();

private:
    // Number of AL buffers in the streaming ring and PCM frames per buffer.
    // 3 buffers * 8192 frames @ 44100 Hz ≈ 0.56 seconds of total queue depth.
    static constexpr int NumStreamBuffers  = 3;
    static constexpr int StreamBufferFrames = 8192;

    struct MusicStream
    {
        drmp3       decoder;
        ALuint      source          = 0;
        ALuint      buffers[NumStreamBuffers] = {};
        bool        looping         = false;
        bool        active          = false;  // decoder open, source & buffers allocated
        std::string path;
    };

    ALCdevice*  device  = nullptr;
    ALCcontext* context = nullptr;

    // One-shot sound state
    std::unordered_map<std::string, SoundId> pathToId;
    std::unordered_map<SoundId, ALuint>      soundBuffers;
    std::vector<ALuint>                      sources;
    uint32_t                                 nextSoundId = 1;
    int                                      maxSources  = 32;

    // Music streaming state
    std::unordered_map<std::string, MusicId> musicPathToId;
    std::unordered_map<MusicId, MusicStream> musicStreams;
    uint32_t                                 nextMusicId = 1;

    // Decodes up to StreamBufferFrames of PCM into an AL buffer.
    // Returns the number of frames decoded; 0 means end-of-file.
    int FillBuffer(MusicStream& stream, ALuint buffer);

    // Returns a one-shot source ready for playback.
    // Allocates a new one if below maxSources, otherwise steals the oldest busy one.
    ALuint AcquireSource();
};
