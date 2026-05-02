#pragma once

#include "Flowstate.h"

#include <memory>

class FlowstateManager
{
public:
    ~FlowstateManager();

    // Queues a state transition. The switch is applied at the start of the
    // next Update() so the current state is never destroyed mid-execution.
    void SwitchTo(std::unique_ptr<Flowstate> newState);

    // Applies any pending transition, then ticks the active state.
    void Update(float deltaTime);

private:
    std::unique_ptr<Flowstate> currentState;
    std::unique_ptr<Flowstate> pendingState;

    void ApplyPendingTransition();
};
