#include "UserSettings/AudioSettings.h"
#include "UserSettings/GlobalSettings.h"
#include "Log.h"

#include <string>

AudioSettings& AudioSettings::Get()
{
    return ServiceLocator::GetGlobalSettings().audio;
}

void AudioSettings::ResetToDefaults()
{
    soundEffectsVolume = 1.0f;
    musicVolume        = 1.0f;
}

void AudioSettings::LogValues() const
{
    Log::Info(LogCategory::UserSettings, "Sound Effects Volume: " + std::to_string(static_cast<int>(soundEffectsVolume * 100.0f)) + "%");
    Log::Info(LogCategory::UserSettings, "Music Volume: "         + std::to_string(static_cast<int>(musicVolume        * 100.0f)) + "%");
}
