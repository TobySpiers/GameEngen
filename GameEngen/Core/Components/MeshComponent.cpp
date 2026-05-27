#include "Components/MeshComponent.h"

#include "GameObject.h"
#include "Rendering/Mesh.h"
#include "Rendering/MeshAsset.h"
#include "Rendering/Renderer.h"
#include "Rendering/Texture.h"

MeshComponent::MeshComponent()
{
    SetTickEnabled(false);
}

void MeshComponent::SetMesh(std::shared_ptr<const MeshAsset> meshAsset)
{
    mesh = std::make_shared<Mesh>(std::move(meshAsset));
}

void MeshComponent::SetTexture(std::shared_ptr<const Texture> texture)
{
    if (mesh)
    {
        mesh->SetTexture(std::move(texture));
    }
}

void MeshComponent::SetTexture(const std::string& path)
{
    if (mesh)
    {
        mesh->SetTexture(path);
    }
}

void MeshComponent::OnDraw(Renderer& renderer)
{
    if (!mesh)
    {
        return;
    }

    renderer.DrawMesh(*mesh, GetOwner()->transform, tint);
}
