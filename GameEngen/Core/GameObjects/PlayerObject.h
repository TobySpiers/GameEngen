#pragma once

#include "GameObject.h"
#include "GameInputs.h"
#include "Audio/AudioTypes.h"

class SpriteComponent;
class SoundComponent;
class SoundListenerComponent;

class PlayerObject : public GameObject
{
public:
    PlayerObject();

    SpriteComponent*        GetSpriteComponent()   const { return spriteComp; }
    SoundComponent*         GetSoundComponent()    const { return soundComp; }
    SoundListenerComponent* GetListenerComponent() const { return listenerComp; }

    float moveSpeed = 200.0f;

protected:
    void OnTick(float deltaTime) override;

private:
    SpriteComponent*        spriteComp    = nullptr;
    SoundComponent*         soundComp     = nullptr;
    SoundListenerComponent* listenerComp  = nullptr;
    SoundId                 startUpSound  = InvalidSound;
};
