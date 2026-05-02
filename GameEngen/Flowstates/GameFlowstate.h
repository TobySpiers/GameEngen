#pragma once

#include "Core/Flowstate.h"
#include "Audio/SoundManager.h"

enum GameInput
{
    Input_PlaySound,
};

class SpriteObject;

class GameFlowstate : public Flowstate
{
public:
    void FlowstateEnter()                 override;
    void FlowstateUpdate(float deltaTime) override;
    void FlowstateExit()                  override;

private:
    SpriteObject* testSprite  = nullptr;
    SoundId       startUpSound = InvalidSound;
    MusicId       musicLoop    = InvalidMusic;
};
