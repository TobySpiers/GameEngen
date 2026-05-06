#include "DebugPanels/DebugPanelLog.h"
#include "Log.h"

#include <imgui.h>

// ---------------------------------------------------------------------------
// Helpers (file-scope)
// ---------------------------------------------------------------------------

static ImVec4 ColorForLevel(LogLevel level)
{
    switch (level)
    {
        case LogLevel::Warning: return ImVec4(1.0f, 0.65f, 0.0f, 1.0f);  // orange
        case LogLevel::Error:   return ImVec4(1.0f, 0.3f,  0.3f, 1.0f);  // red
        default:                return ImGui::GetStyleColorVec4(ImGuiCol_Text);
    }
}

static const char* CategoryLabel(LogCategory category)
{
    switch (category)
    {
        case LogCategory::Common:       return "[Common]";
        case LogCategory::Graphics:     return "[Graphics]";
        case LogCategory::Audio:        return "[Audio]";
        case LogCategory::UserSettings: return "[UserSettings]";
        default:                        return "[?]";
    }
}

// ---------------------------------------------------------------------------
// DebugPanelLog
// ---------------------------------------------------------------------------

void DebugPanelLog::MakePanel()
{
    ImGui::Begin("Log", &bIsOpen);

    // --- Level filters ---
    ImGui::SeparatorText("Level");
    ImGui::Checkbox("Info",    &bShowInfo);    ImGui::SameLine();
    ImGui::Checkbox("Warning", &bShowWarning); ImGui::SameLine();
    ImGui::Checkbox("Error",   &bShowError);

    // --- Category filters ---
    ImGui::SeparatorText("Category");
    ImGui::Checkbox("Common",        &bShowCommon);       ImGui::SameLine();
    ImGui::Checkbox("Graphics",      &bShowGraphics);     ImGui::SameLine();
    ImGui::Checkbox("Audio",         &bShowAudio);        ImGui::SameLine();
    ImGui::Checkbox("User Settings", &bShowUserSettings);

    // --- Clear button, right-aligned ---
    const float clearWidth = ImGui::CalcTextSize("Clear").x + ImGui::GetStyle().FramePadding.x * 2.0f;
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - clearWidth + ImGui::GetCursorPosX());
    if (ImGui::Button("Clear"))
    {
        Log::Clear();
    }

    ImGui::Separator();

    // --- Scrollable log entries ---
    ImGui::BeginChild("LogEntries", ImVec2(0.0f, 0.0f), false, ImGuiWindowFlags_HorizontalScrollbar);

    const std::vector<LogEntry>& entries = Log::GetEntries();

    for (const LogEntry& entry : entries)
    {
        // Level filter
        if (entry.level == LogLevel::Info    && !bShowInfo)    { continue; }
        if (entry.level == LogLevel::Warning && !bShowWarning) { continue; }
        if (entry.level == LogLevel::Error   && !bShowError)   { continue; }

        // Category filter
        if (entry.category == LogCategory::Common       && !bShowCommon)       { continue; }
        if (entry.category == LogCategory::Graphics     && !bShowGraphics)     { continue; }
        if (entry.category == LogCategory::Audio        && !bShowAudio)        { continue; }
        if (entry.category == LogCategory::UserSettings && !bShowUserSettings) { continue; }

        ImGui::TextColored(ColorForLevel(entry.level), "%s %s",
            CategoryLabel(entry.category), entry.message.c_str());
    }

    // Auto-scroll to the bottom when new entries arrive, provided the user
    // hasn't scrolled up to read older entries.
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
    {
        ImGui::SetScrollHereY(1.0f);
    }

    ImGui::EndChild();
    ImGui::End();
}
