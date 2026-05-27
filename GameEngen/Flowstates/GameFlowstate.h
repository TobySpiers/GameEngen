#pragma once

#include "Flowstate.h"
#include "Audio/SoundManager.h"
#include "GameInputs.h"

class PlayerObject;
class StaticMeshObject;

class GameFlowstate : public Flowstate
{
public:
    void FlowstateEnter()                 override;
    void FlowstateUpdate(float deltaTime) override;
    void FlowstateExit()                  override;

private:
    PlayerObject*     player    = nullptr;
    MusicId           musicLoop = InvalidMusic;
    StaticMeshObject* cubeObj    = nullptr;
    StaticMeshObject* cubeObj2  = nullptr;
    float             time      = 0.0f;
};
