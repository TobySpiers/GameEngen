#include "Profiler.h"

#include <chrono>

// ---------------------------------------------------------------------------
// Thread-local scope stack — records the nesting of active ProfileScopes so
// that parent-child relationships are inferred automatically.
// ---------------------------------------------------------------------------
static thread_local std::vector<const char*> s_scopeStack;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

double Profiler::CurrentMs()
{
    using Clock    = std::chrono::high_resolution_clock;
    using Duration = std::chrono::duration<double, std::milli>;
    return Duration(Clock::now().time_since_epoch()).count();
}

// ---------------------------------------------------------------------------
// Profiler
// ---------------------------------------------------------------------------

Profiler& Profiler::Get()
{
    static Profiler instance;
    return instance;
}

void Profiler::BeginFrame()
{
    frameStartMs = CurrentMs();

    for (auto& [name, data] : categories)
    {
        data.accumulatedMs = 0.0;
        data.bActive       = false;
    }
}

void Profiler::EndFrame()
{
    totalFrameMs = CurrentMs() - frameStartMs;

    // Finalise any categories that were still active (Begin without End)
    const double now = CurrentMs();
    for (auto& [name, data] : categories)
    {
        if (data.bActive)
        {
            data.accumulatedMs += now - data.startMs;
            data.bActive        = false;
        }
    }

    // ── Step 1: create a ProfileResult for every category ─────────────────
    // Iterate categoryOrder (first-seen insertion order) so that the tree is
    // built in call order and the display never reorders rows between frames.
    std::unordered_map<std::string, ProfileResult> resultMap;
    for (const std::string& name : categoryOrder)
    {
        resultMap[name] = { name, categories[name].accumulatedMs, {} };
    }

    // ── Step 2: attach each category to its parent's children list ─────────
    for (const std::string& name : categoryOrder)
    {
        const std::string& parent = categories[name].parent;
        if (!parent.empty())
        {
            auto parentIt = resultMap.find(parent);
            if (parentIt != resultMap.end())
            {
                parentIt->second.children.push_back(resultMap[name]);
            }
        }
    }

    // ── Step 3: collect root-level results (no parent, or parent missing) ──
    results.clear();
    for (const std::string& name : categoryOrder)
    {
        const std::string& parent  = categories[name].parent;
        const bool bIsRoot         = parent.empty();
        const bool bOrphaned       = !parent.empty() &&
                                     resultMap.find(parent) == resultMap.end();

        if (bIsRoot || bOrphaned)
        {
            results.push_back(std::move(resultMap[name]));
        }
    }

    // ── Step 4: unknown time = total minus the sum of all root-level ms ─────
    // Root categories already include their children's time, so only sum roots.
    double trackedMs = 0.0;
    for (const ProfileResult& r : results)
    {
        trackedMs += r.ms;
    }
    unknownMs = std::max(0.0, totalFrameMs - trackedMs);
}

void Profiler::Begin(const char* category)
{
    const char* parent = s_scopeStack.empty() ? "" : s_scopeStack.back();

    // Record insertion order on first encounter so EndFrame can iterate in
    // a stable, call-order sequence rather than unordered_map order.
    if (categories.find(category) == categories.end())
    {
        categoryOrder.push_back(category);
    }

    CategoryData& data = categories[category];
    data.startMs       = CurrentMs();
    data.bActive       = true;
    data.parent        = parent;

    s_scopeStack.push_back(category);
}

void Profiler::End(const char* category)
{
    // Pop from the scope stack — only if this is the innermost active scope.
    if (!s_scopeStack.empty() && s_scopeStack.back() == category)
    {
        s_scopeStack.pop_back();
    }

    auto it = categories.find(category);
    if (it == categories.end() || !it->second.bActive)
    {
        return;
    }

    CategoryData& data  = it->second;
    data.accumulatedMs += CurrentMs() - data.startMs;
    data.bActive        = false;
}

const std::vector<ProfileResult>& Profiler::GetResults() const
{
    return results;
}

double Profiler::GetTotalFrameMs() const
{
    return totalFrameMs;
}

double Profiler::GetUnknownMs() const
{
    return unknownMs;
}
