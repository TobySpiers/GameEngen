#include "DebugPanelGraphicsSettings.h"

#include "Log.h"
#include "UserSettings/GraphicsSettings.h"
#include "Rendering/Renderer.h"

#include <imgui.h>
#include <string>

void DebugPanelGraphicsSettings::MakePanel()
{
    GraphicsSettings& gs = GraphicsSettings::Get();
    Renderer&         r  = Renderer::Get();

    ImGui::Begin("Graphics Settings", &bIsOpen);

    ImGui::SeparatorText("Render Target");

    // Build the native label dynamically so it always shows the current window dims
    char nativeLabel[32];
    snprintf(nativeLabel, sizeof(nativeLabel), "%dx%d (native)",
             r.GetWindowWidth(), r.GetWindowHeight());

    // Combine fixed presets with the dynamic native entry for the combo
    const char* allLabels[GraphicsSettings::ResolutionCount + 1];
    for (int i = 0; i < GraphicsSettings::ResolutionCount; ++i)
    {
        allLabels[i] = GraphicsSettings::ResolutionLabels[i];
    }
    allLabels[GraphicsSettings::NativeResolutionIndex] = nativeLabel;

    if (ImGui::Combo("Resolution", &gs.selectedResolution, allLabels, GraphicsSettings::ResolutionCount + 1))
    {
        if (gs.selectedResolution == GraphicsSettings::NativeResolutionIndex)
        {
            r.ResizeRenderTarget(r.GetWindowWidth(), r.GetWindowHeight());
            Log::Info(LogCategory::UserSettings, "Resolution: Native");
        }
        else
        {
            r.ResizeRenderTarget(GraphicsSettings::ResolutionWidths[gs.selectedResolution],
                                 GraphicsSettings::ResolutionHeights[gs.selectedResolution]);
            Log::Info(LogCategory::UserSettings, "Resolution: " + std::string(GraphicsSettings::ResolutionLabels[gs.selectedResolution]));
        }
    }
    ImGui::Text("RT: %dx%d", r.GetRenderTargetWidth(), r.GetRenderTargetHeight());

    ImGui::SeparatorText("Camera");
    if (ImGui::DragFloat("World Units Wide", &gs.worldUnitsWide, 10.0f, 100.0f, 10000.0f, "%.0f"))
    {
        r.UpdateProjection();
        Log::Info(LogCategory::UserSettings, "World Units Wide: " + std::to_string(static_cast<int>(gs.worldUnitsWide)));
    }
    const float worldUnitsHigh = gs.worldUnitsWide /
        (static_cast<float>(r.GetRenderTargetWidth()) / static_cast<float>(r.GetRenderTargetHeight()));
    ImGui::Text("World Units High: %.0f", worldUnitsHigh);

    ImGui::SeparatorText("Display");
    if (ImGui::Checkbox("Stretch to Fill", &gs.bStretchToFill))
    {
        Log::Info(LogCategory::UserSettings, "Stretch to Fill: " + std::string(gs.bStretchToFill ? "true" : "false"));
    }

    ImGui::SeparatorText("Window");
    static constexpr const char* windowModeLabels[] = { "Windowed", "Borderless Windowed", "Fullscreen" };
    int windowModeIndex = static_cast<int>(gs.windowMode);
    if (ImGui::Combo("Window Mode", &windowModeIndex, windowModeLabels, 3))
    {
        r.SetWindowMode(static_cast<WindowMode>(windowModeIndex));
        Log::Info(LogCategory::UserSettings, "Window Mode: " + std::string(windowModeLabels[windowModeIndex]));
    }

    ImGui::End();
}
