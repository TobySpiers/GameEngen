#include "Components/SpriteComponent.h"

#include "GameObject.h"
#include "Rendering/Renderer.h"
#include "Rendering/Texture.h"

#include <glm/glm.hpp>

SpriteComponent::SpriteComponent()
{
    SetTickEnabled(false);
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

    renderer.DrawSprite(*texture, position, size, rotation);
}
