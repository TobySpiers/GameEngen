#pragma once

#include <string>
#include <unordered_map>
#include <vector>

// ---------------------------------------------------------------------------
// ProfileResult — one category's timing for the last complete frame.
// Children are sub-scopes that were nested inside this category.
// ---------------------------------------------------------------------------
struct ProfileResult
{
    std::string                name;
    double                     ms = 0.0;
    std::vector<ProfileResult> children;
};

// ---------------------------------------------------------------------------
// Profiler — static singleton, always accessible (no ServiceLocator needed).
//
// Usage:
//   // Automatic (preferred):
//   { ProfileScope scope("Audio"); ... }
//
//   // Manual:
//   Profiler::Get().Begin("Audio");
//   ...
//   Profiler::Get().End("Audio");
//
// Call BeginFrame() at the top of the game loop and EndFrame() just before
// the buffer swap to measure each frame's total CPU time.
//
// Parent-child relationships are inferred automatically from the call stack:
// any ProfileScope created while another is active becomes its child.
// ---------------------------------------------------------------------------
class Profiler
{
public:
    static Profiler& Get();

    // Call once at the start of each game loop iteration.
    void BeginFrame();

    // Call once at the end of each game loop iteration (before buffer swap).
    // Finalises accumulators, builds the result tree, and computes unknown time.
    void EndFrame();

    // Mark the start/end of a named category within the current frame.
    // The active parent scope (if any) is recorded automatically.
    void Begin(const char* category);
    void End  (const char* category);

    // Top-level results from the last *complete* frame, sorted by ms descending.
    // Each result may contain children (also sorted by ms descending).
    // Does not include Unknown or Total — retrieve those separately.
    const std::vector<ProfileResult>& GetResults()      const;
    double                            GetTotalFrameMs() const;
    double                            GetUnknownMs()    const;

private:
    struct CategoryData
    {
        double      accumulatedMs = 0.0;
        double      startMs       = 0.0;
        bool        bActive       = false;
        std::string parent;               // empty = root level
    };

    static double CurrentMs();

    std::unordered_map<std::string, CategoryData> categories;
    std::vector<std::string>                      categoryOrder; // first-seen insertion order
    std::vector<ProfileResult>                    results;

    double frameStartMs = 0.0;
    double totalFrameMs = 0.0;
    double unknownMs    = 0.0;
};

// ---------------------------------------------------------------------------
// ProfileScope — RAII wrapper that calls Begin on construction, End on
// destruction. Nesting ProfileScopes automatically builds a call tree.
// ---------------------------------------------------------------------------
struct ProfileScope
{
    explicit ProfileScope(const char* category)
        : category(category)
    {
        Profiler::Get().Begin(category);
    }

    ~ProfileScope()
    {
        Profiler::Get().End(category);
    }

    // Non-copyable, non-movable
    ProfileScope(const ProfileScope&)            = delete;
    ProfileScope& operator=(const ProfileScope&) = delete;

private:
    const char* category;
};
