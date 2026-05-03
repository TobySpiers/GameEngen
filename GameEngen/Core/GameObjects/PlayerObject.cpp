#include "Core/GameObjects/PlayerObject.h"

#include "Core/AssetManager.h"
#include "Core/Components/SpriteComponent.h"
#include "Core/InputManager.h"

PlayerObject::PlayerObject(const std::string& texturePath)
    : PlayerObject(AssetManager::Get().GetTexture(texturePath))
{
}

PlayerObject::PlayerObject(std::shared_ptr<const Texture> texture)
{
    spriteComp = AddComponent<SpriteComponent>();
    spriteComp->SetTexture(std::move(texture));
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
}
