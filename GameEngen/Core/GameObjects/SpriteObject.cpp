#include "Core/GameObjects/SpriteObject.h"

#include "Core/AssetManager.h"
#include "Core/Components/SpriteComponent.h"

SpriteObject::SpriteObject(const std::string& texturePath)
    : SpriteObject(AssetManager::Get().GetTexture(texturePath))
{
}

SpriteObject::SpriteObject(std::shared_ptr<const Texture> texture)
{
    spriteComp = AddComponent<SpriteComponent>();
    spriteComp->SetTexture(std::move(texture));
}
