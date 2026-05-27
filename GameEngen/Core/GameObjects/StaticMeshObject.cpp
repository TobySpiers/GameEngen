#include "GameObjects/StaticMeshObject.h"

#include "AssetManager.h"
#include "Components/MeshComponent.h"

StaticMeshObject::StaticMeshObject(std::shared_ptr<const MeshAsset> meshAsset,
                                   std::shared_ptr<const Texture>   texture)
{
    meshComp = AddComponent<MeshComponent>();
    meshComp->SetMesh(std::move(meshAsset));

    if (texture)
    {
        meshComp->SetTexture(std::move(texture));
    }
}

StaticMeshObject::StaticMeshObject(std::shared_ptr<const MeshAsset> meshAsset,
                                   const char*                      texturePath)
    : StaticMeshObject(std::move(meshAsset), AssetManager::Get().GetTexture(texturePath))
{
}
