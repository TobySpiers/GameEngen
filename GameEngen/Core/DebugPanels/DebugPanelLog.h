#pragma once

#include "DebugPanels/DebugPanel.h"

class DebugPanelLog : public DebugPanel
{
public:
    void        MakePanel()         override;
    const char* GetCategory() const override { return "Debug"; }
    const char* GetName()     const override { return "Log"; }

private:
    // Level filters
    bool bShowInfo    = true;
    bool bShowWarning = true;
    bool bShowError   = true;

    // Category filters
    bool bShowCommon       = true;
    bool bShowGraphics     = true;
    bool bShowAudio        = true;
    bool bShowUserSettings = true;
};
