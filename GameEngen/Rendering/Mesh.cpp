#include "Mesh.h"

#include "AssetManager.h"
#include "MeshAsset.h"
#include "Rendering/Texture.h"

Mesh::Mesh(std::shared_ptr<const MeshAsset> asset)
    : meshAsset(std::move(asset))
{
}

void Mesh::SetTexture(std::shared_ptr<const Texture> inTexture)
{
    texture = std::move(inTexture);
}

void Mesh::SetTexture(const std::string& path)
{
    texture = AssetManager::Get().GetTexture(path);
}
