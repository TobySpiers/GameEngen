#pragma once

#include "DebugPanel.h"

class DebugPanelAudioSettings : public DebugPanel
{
public:
    void        MakePanel()         override;
    const char* GetCategory() const override { return "Settings"; }
    const char* GetName()     const override { return "Audio"; }
};
