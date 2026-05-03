#pragma once

#include "Core/Flowstate.h"
#include "Audio/SoundManager.h"
#include "Core/GameInputs.h"

class PlayerObject;

class GameFlowstate : public Flowstate
{
public:
    void FlowstateEnter()                 override;
    void FlowstateUpdate(float deltaTime) override;
    void FlowstateExit()                  override;

private:
    PlayerObject* player      = nullptr;
    SoundId       startUpSound = InvalidSound;
    MusicId       musicLoop    = InvalidMusic;
};
