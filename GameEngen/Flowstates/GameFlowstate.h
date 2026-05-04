#pragma once

#include "Flowstate.h"
#include "Audio/SoundManager.h"
#include "GameInputs.h"

class PlayerObject;

class GameFlowstate : public Flowstate
{
public:
    void FlowstateEnter()                 override;
    void FlowstateUpdate(float deltaTime) override;
    void FlowstateExit()                  override;

private:
    PlayerObject* player    = nullptr;
    MusicId       musicLoop = InvalidMusic;
};
