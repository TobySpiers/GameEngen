#pragma once

#include "DebugPanel.h"
#include "Core/ServiceLocator.h"

#include <memory>
#include <type_traits>
#include <vector>

class DebugPanelManager
{
public:
    static DebugPanelManager& Get() { return ServiceLocator::GetDebugPanelManager(); }

public:
    // Constructs a panel of type T in-place and registers it.
    // T must derive from DebugPanel.
    template <typename T, typename... Args>
    T* RegisterPanel(Args&&... args)
    {
        static_assert(std::is_base_of<DebugPanel, T>::value, "T must derive from DebugPanel");
        auto panel = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = panel.get();
        panels.push_back(std::move(panel));
        return ptr;
    }

    // Removes the panel that was returned by RegisterPanel. Safe to call from
    // FlowstateExit — the pointer is invalidated after this returns.
    void UnregisterPanel(DebugPanel* panel);

    // Renders the ImGui main menu bar and calls MakePanel() on all open panels.
    // Call once per frame after game rendering, before ImGui::Render().
    void Update();

private:
    std::vector<std::unique_ptr<DebugPanel>> panels;
};
