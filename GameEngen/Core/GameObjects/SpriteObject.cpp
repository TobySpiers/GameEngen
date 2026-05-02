#include "Core/GameObjects/SpriteObject.h"

#include "Core/Components/SpriteComponent.h"

SpriteObject::SpriteObject(const std::string& texturePath)
{
    texture    = std::make_unique<Texture>(texturePath);
    spriteComp = AddComponent<SpriteComponent>();
    spriteComp->SetTexture(texture.get());
}
