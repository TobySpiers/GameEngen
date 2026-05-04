#include "UserSettings/AudioSettings.h"
#include "UserSettings/GlobalSettings.h"

AudioSettings& AudioSettings::Get()
{
    return ServiceLocator::GetGlobalSettings().audio;
}

void AudioSettings::ResetToDefaults()
{
    soundEffectsVolume = 1.0f;
    musicVolume        = 1.0f;
}
