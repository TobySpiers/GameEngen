#include "UserSettings/GraphicsSettings.h"
#include "UserSettings/GlobalSettings.h"

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
