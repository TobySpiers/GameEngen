#include "FlowstateManager.h"

#include "AssetManager.h"
#include "Profiler.h"

FlowstateManager::~FlowstateManager()
{
    // Ensure the active state gets a chance to clean up
    if (currentState)
    {
        currentState->FlowstateExit();
        AssetManager::Get().UnloadAllAssets();
    }
}

void FlowstateManager::SwitchTo(std::unique_ptr<Flowstate> newState)
{
    pendingState = std::move(newState);
}

void FlowstateManager::Update(float deltaTime)
{
    ApplyPendingTransition();

    if (currentState)
    {
        ProfileScope scope("FlowstateUpdate");
        currentState->FlowstateUpdate(deltaTime);
    }
}

void FlowstateManager::ApplyPendingTransition()
{
    if (!pendingState)
    {
        return;
    }

    if (currentState)
    {
        currentState->FlowstateExit();
        AssetManager::Get().UnloadAllAssets();
    }

    currentState = std::move(pendingState);
    currentState->FlowstateEnter();
}
