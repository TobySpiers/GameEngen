#include "DebugPanels/DebugPanelProfiler.h"
#include "Profiler.h"

#include <imgui.h>

#include <string>

// ---------------------------------------------------------------------------
// Helpers (file-scope)
// ---------------------------------------------------------------------------

// Generates a distinct colour for a named category using a simple string hash
// mapped onto the HSV colour wheel.
static ImVec4 CategoryColor(const std::string& name)
{
    uint32_t hash = 0;
    for (char c : name)
    {
        hash = hash * 31 + static_cast<unsigned char>(c);
    }
    float hue = static_cast<float>(hash % 360) / 360.0f;
    float r, g, b;
    ImGui::ColorConvertHSVtoRGB(hue, 0.60f, 0.90f, r, g, b);
    return ImVec4(r, g, b, 1.0f);
}

// Fills the bar, ms, and % columns for the current table row.
// Column 0 (name) must already have been populated by the caller.
static void DrawRowData(double ms, double budgetMs, ImVec4 color)
{
    const float fraction    = static_cast<float>(ms / budgetMs);
    const bool  bOverBudget = fraction > 1.0f;
    const ImVec4 barColor   = bOverBudget ? ImVec4(1.0f, 0.25f, 0.25f, 1.0f) : color;

    ImGui::TableSetColumnIndex(1);
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, barColor);
    ImGui::ProgressBar(std::min(fraction, 1.0f), ImVec2(-1.0f, 0.0f), "");
    ImGui::PopStyleColor();

    ImGui::TableSetColumnIndex(2);
    ImGui::Text("%.2f ms", ms);

    ImGui::TableSetColumnIndex(3);
    if (bOverBudget)
    {
        ImGui::TextColored(ImVec4(1.0f, 0.25f, 0.25f, 1.0f), "%.1f%%", fraction * 100.0f);
    }
    else
    {
        ImGui::Text("%.1f%%", fraction * 100.0f);
    }
}

// Draws a leaf row that cannot be expanded (used for categories with no
// children, and for the synthetic Unknown / Total rows).
static void DrawLeafRow(const char* name, double ms, double budgetMs, ImVec4 color)
{
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::TreeNodeEx(name,
        ImGuiTreeNodeFlags_Leaf |
        ImGuiTreeNodeFlags_NoTreePushOnOpen |
        ImGuiTreeNodeFlags_SpanAllColumns);
    DrawRowData(ms, budgetMs, color);
}

// Recursively draws a ProfileResult.  Parent nodes show an expand arrow and
// default to open; leaf nodes have no arrow.
// unknownThresholdMs: inner- and root-level Unknown rows are hidden when their
// time falls below this value.
static void DrawResultTree(const ProfileResult& result, double budgetMs, double unknownThresholdMs)
{
    const ImVec4 color = CategoryColor(result.name);

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);

    if (result.children.empty())
    {
        ImGui::TreeNodeEx(result.name.c_str(),
            ImGuiTreeNodeFlags_Leaf |
            ImGuiTreeNodeFlags_NoTreePushOnOpen |
            ImGuiTreeNodeFlags_SpanAllColumns);
        DrawRowData(result.ms, budgetMs, color);
    }
    else
    {
        const bool bOpen = ImGui::TreeNodeEx(result.name.c_str(),
            ImGuiTreeNodeFlags_DefaultOpen |
            ImGuiTreeNodeFlags_SpanAllColumns);
        DrawRowData(result.ms, budgetMs, color);

        if (bOpen)
        {
            for (const ProfileResult& child : result.children)
            {
                DrawResultTree(child, budgetMs, unknownThresholdMs);
            }

            // Show any time inside this scope not covered by a named child.
            double childrenMs = 0.0;
            for (const ProfileResult& child : result.children)
            {
                childrenMs += child.ms;
            }
            const double innerUnknownMs = result.ms - childrenMs;
            if (innerUnknownMs > unknownThresholdMs)
            {
                DrawLeafRow("Unknown", innerUnknownMs, budgetMs,
                            ImVec4(0.55f, 0.55f, 0.55f, 1.0f));
            }

            ImGui::TreePop();
        }
    }
}

// ---------------------------------------------------------------------------
// DebugPanelProfiler
// ---------------------------------------------------------------------------

void DebugPanelProfiler::MakePanel()
{
    ImGui::Begin("Profiler", &bIsOpen);

    // --- Target FPS ---
    ImGui::SliderFloat("Target FPS", &targetFps, 10.0f, 240.0f, "%.0f");
    const double budgetMs = 1000.0 / static_cast<double>(targetFps);
    ImGui::Text("Frame budget: %.2f ms", budgetMs);

    ImGui::SliderFloat("Hide unknown below", &unknownThresholdMs, 0.0f, 5.0f, "%.2f ms");

    ImGui::Separator();

    // --- Table ---
    constexpr ImGuiTableFlags tableFlags =
        ImGuiTableFlags_BordersInnerV |
        ImGuiTableFlags_RowBg         |
        ImGuiTableFlags_SizingFixedFit;

    if (ImGui::BeginTable("ProfileTable", 4, tableFlags))
    {
        ImGui::TableSetupColumn("Category", ImGuiTableColumnFlags_WidthFixed,   160.0f);
        ImGui::TableSetupColumn("",         ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Time",     ImGuiTableColumnFlags_WidthFixed,    72.0f);
        ImGui::TableSetupColumn("%",        ImGuiTableColumnFlags_WidthFixed,    52.0f);
        ImGui::TableHeadersRow();

        const Profiler& profiler = Profiler::Get();

        // Tracked categories (with nesting)
        for (const ProfileResult& result : profiler.GetResults())
        {
            DrawResultTree(result, budgetMs, static_cast<double>(unknownThresholdMs));
        }

        // Root-level unknown (untracked time outside all named scopes)
        const double rootUnknownMs = profiler.GetUnknownMs();
        if (rootUnknownMs > static_cast<double>(unknownThresholdMs))
        {
            DrawLeafRow("Unknown", rootUnknownMs, budgetMs,
                        ImVec4(0.55f, 0.55f, 0.55f, 1.0f));
        }

        // Separator before Total
        ImGui::TableNextRow();
        for (int i = 0; i < 4; ++i)
        {
            ImGui::TableSetColumnIndex(i);
            ImGui::Separator();
        }

        // Total — always at the bottom
        DrawLeafRow("Total", profiler.GetTotalFrameMs(), budgetMs,
                    ImVec4(0.85f, 0.85f, 0.85f, 1.0f));

        ImGui::EndTable();
    }

    ImGui::End();
}
