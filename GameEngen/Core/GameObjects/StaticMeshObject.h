#pragma once

#include "GameObject.h"

#include <memory>

class MeshAsset;
class MeshComponent;
class Texture;

class StaticMeshObject : public GameObject
{
public:
    explicit StaticMeshObject(std::shared_ptr<const MeshAsset> meshAsset,
                              std::shared_ptr<const Texture>   texture = nullptr);

    explicit StaticMeshObject(std::shared_ptr<const MeshAsset> meshAsset,
                              const char*                      texturePath);

    MeshComponent* GetMeshComponent() const { return meshComp; }

private:
    MeshComponent* meshComp = nullptr;
};
