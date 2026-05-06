#include "DebugPanelManager.h"
#include "DebugPanel.h"

#include "UserSettings/GraphicsSettings.h"

#include <imgui.h>

#include <algorithm>
#include <cstring>

// Captured once on the first Update() call, after ImGui is fully initialised.
// Used to restore default sizes before reapplying a fresh scale each time.
static ImGuiStyle s_baseStyle;
static bool       s_bBaseStyleCaptured = false;

static void ApplyUiScale(float scale)
{
    ImGui::GetStyle() = s_baseStyle;
    ImGui::GetStyle().ScaleAllSizes(scale);
    ImGui::GetIO().FontGlobalScale = scale;
}

void DebugPanelManager::UnregisterPanel(DebugPanel* panel)
{
    auto it = std::find_if(panels.begin(), panels.end(),
        [panel](const std::unique_ptr<DebugPanel>& entry)
        {
            return entry.get() == panel;
        });

    if (it != panels.end())
    {
        panels.erase(it);
    }
}

void DebugPanelManager::Update()
{
    // Capture the unmodified default style on the very first frame
    if (!s_bBaseStyleCaptured)
    {
        s_baseStyle            = ImGui::GetStyle();
        s_bBaseStyleCaptured   = true;

        // Apply whatever scale is already stored (e.g. loaded from disk)
        const float storedScale = GraphicsSettings::Get().uiScale;
        if (storedScale != 1.0f)
        {
            ApplyUiScale(storedScale);
        }
    }

    if (ImGui::BeginMainMenuBar())
    {
        // ── Left side: one menu per category ──────────────────────────────

        // Collect unique category names in first-seen order
        std::vector<const char*> categories;
        for (const auto& panel : panels)
        {
            const char* category = panel->GetCategory();
            bool bFound = false;
            for (const char* existing : categories)
            {
                if (strcmp(existing, category) == 0)
                {
                    bFound = true;
                    break;
                }
            }
            if (!bFound)
            {
                categories.push_back(category);
            }
        }

        for (const char* category : categories)
        {
            if (ImGui::BeginMenu(category))
            {
                for (auto& panel : panels)
                {
                    if (strcmp(panel->GetCategory(), category) == 0)
                    {
                        ImGui::MenuItem(panel->GetName(), nullptr, &panel->bIsOpen);
                    }
                }
                ImGui::EndMenu();
            }
        }

        // ── Right side: UI scale slider ────────────────────────────────────

        static constexpr float sliderWidth = 140.0f;
        static constexpr const char* scaleLabel = "UI Scale";
        const float labelWidth  = ImGui::CalcTextSize(scaleLabel).x;
        const float totalWidth  = labelWidth
                                + ImGui::GetStyle().ItemSpacing.x
                                + sliderWidth
                                + ImGui::GetStyle().WindowPadding.x;

        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - totalWidth);
        ImGui::TextUnformatted(scaleLabel);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(sliderWidth);

        float uiScale = GraphicsSettings::Get().uiScale;
        if (ImGui::SliderFloat("##uiScale", &uiScale, 0.5f, 3.0f, "%.1fx"))
        {
            GraphicsSettings::Get().uiScale = uiScale;
            ApplyUiScale(uiScale);
        }

        ImGui::EndMainMenuBar();
    }

    // Draw each open panel
    for (auto& panel : panels)
    {
        if (panel->bIsOpen)
        {
            panel->MakePanel();
        }
    }
}
