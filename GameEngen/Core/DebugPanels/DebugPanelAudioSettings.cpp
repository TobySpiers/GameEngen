#include "DebugPanelAudioSettings.h"

#include "Log.h"
#include "UserSettings/AudioSettings.h"

#include <imgui.h>
#include <string>

void DebugPanelAudioSettings::MakePanel()
{
    AudioSettings& as = AudioSettings::Get();

    ImGui::Begin("Audio Settings", &bIsOpen);

    ImGui::SeparatorText("Volume");

    float sfxPercent = as.soundEffectsVolume * 100.0f;
    if (ImGui::SliderFloat("Sound Effects", &sfxPercent, 0.0f, 100.0f, "%.0f%%"))
    {
        as.soundEffectsVolume = sfxPercent / 100.0f;
        Log::Info(LogCategory::UserSettings, "Sound Effects Volume: " + std::to_string(static_cast<int>(sfxPercent)) + "%");
    }

    float musicPercent = as.musicVolume * 100.0f;
    if (ImGui::SliderFloat("Music", &musicPercent, 0.0f, 100.0f, "%.0f%%"))
    {
        as.musicVolume = musicPercent / 100.0f;
        Log::Info(LogCategory::UserSettings, "Music Volume: " + std::to_string(static_cast<int>(musicPercent)) + "%");
    }

    ImGui::End();
}
