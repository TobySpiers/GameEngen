#include "FlowstateManager.h"

FlowstateManager::~FlowstateManager()
{
    // Ensure the active state gets a chance to clean up
    if (currentState)
    {
        currentState->FlowstateExit();
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
    }

    currentState = std::move(pendingState);
    currentState->FlowstateEnter();
}
