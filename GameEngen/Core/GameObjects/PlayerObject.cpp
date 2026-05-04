#include "GameObjects/PlayerObject.h"

#include "Audio/SoundManager.h"
#include "Components/SoundComponent.h"
#include "Components/SoundListenerComponent.h"
#include "Components/SpriteComponent.h"
#include "InputManager.h"

PlayerObject::PlayerObject()
{
    spriteComp = AddComponent<SpriteComponent>();
    spriteComp->SetTexture("sprites/test.png");

    soundComp    = AddComponent<SoundComponent>();
    listenerComp = AddComponent<SoundListenerComponent>();
    startUpSound = SoundManager::Get().LoadSound("sounds/StartUp.wav");
}

void PlayerObject::OnTick(float deltaTime)
{
    glm::vec3 movement(0.0f);

    if (InputManager::Get().IsKeyDown(Input_MoveLeft))
    {
        movement.x -= 1.0f;
    }
    if (InputManager::Get().IsKeyDown(Input_MoveRight))
    {
        movement.x += 1.0f;
    }
    if (InputManager::Get().IsKeyDown(Input_MoveDown))
    {
        movement.y -= 1.0f;
    }
    if (InputManager::Get().IsKeyDown(Input_MoveUp))
    {
        movement.y += 1.0f;
    }

    if (movement.x != 0.0f || movement.y != 0.0f)
    {
        movement = glm::normalize(movement);
        transform.position += movement * moveSpeed * deltaTime;
    }

    if (InputManager::Get().IsKeyPressed(Input_PlaySound))
    {
        soundComp->PlaySound(startUpSound);
    }
}
