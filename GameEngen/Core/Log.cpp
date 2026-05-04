#include "Log.h"

#include <cstdio>
#include <filesystem>
#include <fstream>

// ---------------------------------------------------------------------------
// File-scope state
// ---------------------------------------------------------------------------

static std::vector<LogEntry> s_entries;
static std::ofstream         s_logFile;
static bool                  s_bFileOpen = false;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static void EnsureFileOpen()
{
    if (!s_bFileOpen)
    {
        std::filesystem::create_directories(SAVED_DIR);
        s_logFile.open(SAVED_DIR "GameEngen.log", std::ios::out | std::ios::trunc);
        s_bFileOpen = s_logFile.is_open();
    }
}

static const char* LevelPrefix(LogLevel level)
{
    switch (level)
    {
        case LogLevel::Info:    return "[Info]   ";
        case LogLevel::Warning: return "[Warning]";
        case LogLevel::Error:   return "[Error]  ";
        default:                return "[?]      ";
    }
}

static const char* CategoryPrefix(LogCategory category)
{
    switch (category)
    {
        case LogCategory::Common:       return "[Common]      ";
        case LogCategory::Graphics:     return "[Graphics]    ";
        case LogCategory::Audio:        return "[Audio]       ";
        case LogCategory::UserSettings: return "[UserSettings]";
        default:                        return "[?]           ";
    }
}

// ---------------------------------------------------------------------------
// Log implementation
// ---------------------------------------------------------------------------

void Log::Write(LogLevel level, LogCategory category, const std::string& message)
{
#if !defined(NDEBUG)
    s_entries.push_back({ level, category, message });
#endif

    const char* levelPrefix    = LevelPrefix(level);
    const char* categoryPrefix = CategoryPrefix(category);

    // Print to console — errors go to stderr, everything else to stdout
    FILE* stream = (level == LogLevel::Error) ? stderr : stdout;
    fprintf(stream, "%s %s %s\n", categoryPrefix, levelPrefix, message.c_str());

    // Write to file, flushing immediately so the log is readable mid-run
    EnsureFileOpen();
    if (s_bFileOpen)
    {
        s_logFile << categoryPrefix << " " << levelPrefix << " " << message << "\n";
        s_logFile.flush();
    }
}

void Log::Info   (LogCategory category, const std::string& message) { Write(LogLevel::Info,    category, message); }
void Log::Warning(LogCategory category, const std::string& message) { Write(LogLevel::Warning, category, message); }
void Log::Error  (LogCategory category, const std::string& message) { Write(LogLevel::Error,   category, message); }

const std::vector<LogEntry>& Log::GetEntries() { return s_entries; }

void Log::Clear() { s_entries.clear(); }
