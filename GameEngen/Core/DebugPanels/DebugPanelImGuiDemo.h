#pragma once

#include "DebugPanel.h"

class DebugPanelImGuiDemo : public DebugPanel
{
public:
    void        MakePanel()         override;
    const char* GetCategory() const override { return "Debug"; }
    const char* GetName()     const override { return "ImGui Demo"; }
};
