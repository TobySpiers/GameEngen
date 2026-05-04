#pragma once

#include "GameObjectComponent.h"

#include <memory>
#include <string>

class Texture;

class SpriteComponent : public GameObjectComponent
{
public:
    SpriteComponent();

    void OnDraw(Renderer& renderer) override;

    void SetTexture(std::shared_ptr<const Texture> inTexture) { texture = std::move(inTexture); }
    void SetTexture(const std::string& path);
    const Texture* GetTexture() const { return texture.get(); }

private:
    std::shared_ptr<const Texture> texture;
};
