#include "Core/GraphicsSettings.h"

void GraphicsSettings::ResetToDefaults()
{
    selectedResolution = NativeResolutionIndex;
    worldUnitsWide     = 1280.0f;
    bStretchToFill     = false;
    windowMode         = WindowMode::Windowed;
    uiScale            = 1.5f;
}
