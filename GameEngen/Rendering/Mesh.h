#pragma once

#include <memory>
#include <string>

class MeshAsset;
class Texture;

class Mesh
{
public:
    explicit Mesh(std::shared_ptr<const MeshAsset> asset);

    void SetTexture(std::shared_ptr<const Texture> inTexture);

    // Looks up the texture by path via AssetManager.
    void SetTexture(const std::string& path);

    const MeshAsset* GetMeshAsset() const { return meshAsset.get(); }
    const Texture*   GetTexture()   const { return texture.get(); }

private:
    std::shared_ptr<const MeshAsset> meshAsset;
    std::shared_ptr<const Texture>   texture;   // nullptr → renderer uses white fallback
};
