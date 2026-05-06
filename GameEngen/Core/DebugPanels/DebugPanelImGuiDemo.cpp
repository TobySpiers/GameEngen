#include "DebugPanelImGuiDemo.h"

#include <imgui.h>

void DebugPanelImGuiDemo::MakePanel()
{
    // Pass &bIsOpen so the window's close button unticks the menu item
    ImGui::ShowDemoWindow(&bIsOpen);
}
