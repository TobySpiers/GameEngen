#pragma once

#include "DebugPanels/DebugPanel.h"

class DebugPanelProfiler : public DebugPanel
{
public:
    void        MakePanel()         override;
    const char* GetCategory() const override { return "Debug"; }
    const char* GetName()     const override { return "Profiler"; }

private:
    float targetFps          = 60.0f;
    float unknownThresholdMs = 0.0f;
};
