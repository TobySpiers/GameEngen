#pragma once

#include "Core/ServiceLocator.h"
#include "Core/UserSettings.h"

enum class WindowMode
{
    Windowed,
    BorderlessWindowed,
    Fullscreen,
};

class GraphicsSettings : public UserSettings
{
public:
    static GraphicsSettings& Get() { return ServiceLocator::GetGraphicsSettings(); }

    void ResetToDefaults() override;

    // Render target resolution presets.
    // NativeResolutionIndex is a sentinel meaning "match the window framebuffer size".
    static constexpr int         ResolutionCount       = 4;
    static constexpr const char* ResolutionLabels[]    = { "1024x768", "1280x720", "1920x1080", "3840x2160" };
    static constexpr int         ResolutionWidths[]    = { 1024, 1280, 1920, 3840 };
    static constexpr int         ResolutionHeights[]   = { 768,  720,  1080, 2160 };
    static constexpr int         NativeResolutionIndex = ResolutionCount;

    int        selectedResolution = NativeResolutionIndex;
    float      worldUnitsWide     = 1280.0f;
    bool       bStretchToFill     = false;
    WindowMode windowMode         = WindowMode::Windowed;
    float      uiScale            = 1.5f;
};
