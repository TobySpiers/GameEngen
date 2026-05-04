#include "UserSettings/GraphicsSettings.h"
#include "UserSettings/GlobalSettings.h"
#include "Log.h"

#include <string>

GraphicsSettings& GraphicsSettings::Get()
{
    return ServiceLocator::GetGlobalSettings().graphics;
}

void GraphicsSettings::ResetToDefaults()
{
    selectedResolution = NativeResolutionIndex;
    worldUnitsWide     = 1280.0f;
    bStretchToFill     = false;
    windowMode         = WindowMode::Windowed;
    uiScale            = 1.5f;
}

static const char* WindowModeToString(WindowMode mode)
{
    switch (mode)
    {
        case WindowMode::Windowed:           return "Windowed";
        case WindowMode::BorderlessWindowed: return "Borderless Windowed";
        case WindowMode::Fullscreen:         return "Fullscreen";
        default:                             return "Unknown";
    }
}

void GraphicsSettings::LogValues() const
{
    if (selectedResolution == NativeResolutionIndex)
        Log::Info(LogCategory::UserSettings, "Resolution: Native");
    else
        Log::Info(LogCategory::UserSettings, "Resolution: " + std::string(ResolutionLabels[selectedResolution]));

    Log::Info(LogCategory::UserSettings, "World Units Wide: " + std::to_string(static_cast<int>(worldUnitsWide)));
    Log::Info(LogCategory::UserSettings, "Stretch to Fill: "  + std::string(bStretchToFill ? "true" : "false"));
    Log::Info(LogCategory::UserSettings, "Window Mode: "      + std::string(WindowModeToString(windowMode)));
    Log::Info(LogCategory::UserSettings, "UI Scale: "         + std::to_string(uiScale));
}
