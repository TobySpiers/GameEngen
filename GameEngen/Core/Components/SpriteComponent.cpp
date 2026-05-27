#include "Components/SpriteComponent.h"

#include "AssetManager.h"
#include "GameObject.h"
#include "Rendering/Renderer.h"
#include "Rendering/Texture.h"

#include <glm/glm.hpp>

SpriteComponent::SpriteComponent()
{
    SetTickEnabled(false);
}

void SpriteComponent::SetTexture(const std::string& path)
{
    texture = AssetManager::Get().GetTexture(path);
}

void SpriteComponent::OnDraw(Renderer& renderer)
{
    if (!texture)
    {
        return;
    }

    const Transform& t = GetOwner()->transform;

    glm::vec2 position = { t.position.x, t.position.y };
    glm::vec2 size     = { texture->GetWidth()  * t.scale.x,
                           texture->GetHeight() * t.scale.y };
    float rotation     = t.rotation.z;

    renderer.DrawSprite(*texture, position, size, rotation, glm::vec4(1.0f), t.position.z);
}
