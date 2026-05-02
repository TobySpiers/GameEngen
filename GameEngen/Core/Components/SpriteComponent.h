#pragma once

#include "Core/GameObjectComponent.h"

class Texture;

class SpriteComponent : public GameObjectComponent
{
public:
    SpriteComponent();

    void OnDraw(Renderer& renderer) override;

    void     SetTexture(Texture* inTexture) { texture = inTexture; }
    Texture* GetTexture() const             { return texture; }

private:
    Texture* texture = nullptr;
};
