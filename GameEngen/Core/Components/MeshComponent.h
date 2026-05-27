#pragma once

#include "GameObjectComponent.h"

#include <glm/glm.hpp>

#include <memory>
#include <string>

class Mesh;
class MeshAsset;

class MeshComponent : public GameObjectComponent
{
public:
    MeshComponent();

    void OnDraw(Renderer& renderer) override;

    // Wraps the given asset in a new Mesh and takes ownership of it.
    void SetMesh(std::shared_ptr<const MeshAsset> meshAsset);

    // Assigns a texture to the current mesh. SetMesh must be called first.
    void SetTexture(std::shared_ptr<const class Texture> texture);
    void SetTexture(const std::string& path);

    Mesh* GetMesh() const { return mesh.get(); }

    glm::vec4 tint = glm::vec4(1.0f);

private:
    std::shared_ptr<Mesh> mesh;
};
