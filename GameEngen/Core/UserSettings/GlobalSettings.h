#pragma once

#include "ServiceLocator.h"
#include "UserSettings/AudioSettings.h"
#include "UserSettings/GraphicsSettings.h"

class GlobalSettings
{
public:
    static GlobalSettings& Get() { return ServiceLocator::GetGlobalSettings(); }

    AudioSettings    audio;
    GraphicsSettings graphics;
};
