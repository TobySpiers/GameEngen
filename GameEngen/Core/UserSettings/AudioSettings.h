#pragma once

#include "UserSettings/UserSettings.h"

class AudioSettings : public UserSettings
{
public:
    static AudioSettings& Get();

    void ResetToDefaults() override;

    float soundEffectsVolume = 1.0f;  // 0.0 to 1.0, maps directly to AL_GAIN
    float musicVolume        = 1.0f;
};
