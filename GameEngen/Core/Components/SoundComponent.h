#pragma once

#include "GameObjectComponent.h"
#include "Audio/AudioTypes.h"

#include <string>

class SoundComponent : public GameObjectComponent
{
public:
    SoundComponent();

    // Play a previously loaded sound by ID.
    void PlaySound(SoundId id);

    // Play a sound directly from a filepath (loads and caches via SoundManager).
    void PlaySound(const std::string& path);

    // Start playing a previously loaded music track by ID.
    void PlayMusic(MusicId id, bool bLoop = true);

    // Start playing a music track directly from a filepath (loads and caches via SoundManager).
    void PlayMusic(const std::string& path, bool bLoop = true);

    // Stop a playing music track.
    void StopMusic(MusicId id);
};
