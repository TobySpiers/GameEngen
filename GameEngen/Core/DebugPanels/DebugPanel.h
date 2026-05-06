#pragma once

class DebugPanel
{
public:
    virtual ~DebugPanel() = default;

    // Renders the ImGui window contents. Called once per frame while bIsOpen is true.
    virtual void MakePanel() = 0;

    // The top-level menu name this panel appears under (e.g. "Settings").
    virtual const char* GetCategory() const = 0;

    // The menu item label used to toggle this panel (e.g. "Graphics").
    virtual const char* GetName() const = 0;

    bool bIsOpen = false;
};
