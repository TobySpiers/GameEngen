#pragma once

#include "Core/GameObject.h"
#include "Rendering/Texture.h"

#include <memory>
#include <string>

class SpriteComponent;

class SpriteObject : public GameObject
{
public:
    explicit SpriteObject(const std::string& texturePath);

    SpriteComponent* GetSpriteComponent() const { return spriteComp; }

private:
    std::unique_ptr<Texture> texture;
    SpriteComponent*         spriteComp = nullptr;
};
