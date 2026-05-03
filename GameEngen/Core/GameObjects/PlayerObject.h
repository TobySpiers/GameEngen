#pragma once

#include "Core/GameObject.h"
#include "Core/GameInputs.h"

#include <memory>
#include <string>

class SpriteComponent;
class Texture;

class PlayerObject : public GameObject
{
public:
    explicit PlayerObject(const std::string& texturePath);
    explicit PlayerObject(std::shared_ptr<const Texture> texture);

    SpriteComponent* GetSpriteComponent() const { return spriteComp; }

    float moveSpeed = 200.0f;

protected:
    void OnTick(float deltaTime) override;

private:
    SpriteComponent* spriteComp = nullptr;
};
