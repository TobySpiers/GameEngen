#pragma once

#include "Core/GameObjectComponent.h"

#include <memory>

class Texture;

class SpriteComponent : public GameObjectComponent
{
public:
    SpriteComponent();

    void OnDraw(Renderer& renderer) override;

    void SetTexture(std::shared_ptr<const Texture> inTexture) { texture = std::move(inTexture); }
    const Texture* GetTexture() const                         { return texture.get(); }

private:
    std::shared_ptr<const Texture> texture;
};
