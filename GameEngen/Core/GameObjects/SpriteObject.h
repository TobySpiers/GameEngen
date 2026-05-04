#pragma once

#include "GameObject.h"

#include <memory>
#include <string>

class SpriteComponent;
class Texture;

class SpriteObject : public GameObject
{
public:
    explicit SpriteObject(const std::string& texturePath);
    explicit SpriteObject(std::shared_ptr<const Texture> texture);

    SpriteComponent* GetSpriteComponent() const { return spriteComp; }

private:
    SpriteComponent* spriteComp = nullptr;
};
