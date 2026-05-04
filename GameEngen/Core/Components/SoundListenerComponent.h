#pragma once

#include "GameObjectComponent.h"

// Represents the OpenAL listener in world space.
// Only one SoundListenerComponent may be active at a time — if a second is spawned,
// it replaces the first and a warning is printed.
// Tick is enabled so the listener position/orientation is pushed to OpenAL every frame.
class SoundListenerComponent : public GameObjectComponent
{
public:
    SoundListenerComponent();

protected:
    void OnSpawn() override;
    void OnTick(float deltaTime) override;
    void OnDestroy() override;

private:
    // Pushes the owner's transform (position + orientation derived from Euler rotation)
    // to the OpenAL listener state.
    void ApplyToAL();

    static SoundListenerComponent* s_activeListener;
};
