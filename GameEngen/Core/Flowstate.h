#pragma once

class Flowstate
{
public:
    virtual ~Flowstate() = default;

    virtual void FlowstateEnter() {}
    virtual void FlowstateUpdate(float deltaTime) = 0;
    virtual void FlowstateExit() {}
};
