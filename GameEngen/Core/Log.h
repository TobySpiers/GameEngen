#pragma once

#include <string>
#include <vector>

enum class LogLevel    { Info, Warning, Error };
enum class LogCategory { Common, Graphics, Audio, UserSettings };

struct LogEntry
{
    LogLevel    level;
    LogCategory category;
    std::string message;
};

class Log
{
public:
    static void Info   (LogCategory category, const std::string& message);
    static void Warning(LogCategory category, const std::string& message);
    static void Error  (LogCategory category, const std::string& message);

    // Returns all entries accumulated since startup (or the last Clear).
    // Intended for the future ImGui log panel.
    static const std::vector<LogEntry>& GetEntries();

    static void Clear();

private:
    static void Write(LogLevel level, LogCategory category, const std::string& message);
};

// ---------------------------------------------------------------------------
// Ensure
// ---------------------------------------------------------------------------
// Returns true if condition holds.
// On failure: logs the message as an error and, in non-release builds,
// breaks into the debugger before returning false.
// Usage:  Ensure(ptr != nullptr, "ptr must not be null");
//         if (!Ensure(ptr != nullptr, "ptr must not be null")) { return; }
// ---------------------------------------------------------------------------

inline bool EnsureFailed(const char* message)
{
    Log::Error(LogCategory::Common, message);
#if !defined(NDEBUG)
    __debugbreak();
#endif
    return false;
}

#define Ensure(condition, message) \
    ((condition) ? true : EnsureFailed(message))
