#include "GameObject.h"
#include "GameObjectManager.h"
#include "ServiceLocator.h"

void GameObject::SetTickEnabled(bool bEnabled)
{
    if (bTickEnabled == bEnabled)
    {
        return;
    }
    bTickEnabled = bEnabled;
    GameObjectManager::Get().OnTickEnabledChanged(this, bEnabled);
}

void GameObject::SetDrawEnabled(bool bEnabled)
{
    if (bDrawEnabled == bEnabled)
    {
        return;
    }
    bDrawEnabled = bEnabled;
    GameObjectManager::Get().OnDrawEnabledChanged(this, bEnabled);
}

void GameObject::Destroy()
{
    if (bPendingDestroy)
    {
        return;
    }
    bPendingDestroy = true;
    GameObjectManager::Get().RequestDestroy(this);
}

void GameObject::ExecuteSpawn()
{
    bHasSpawned = true;

    // Spawn components registered before this object was spawned (e.g. via
    // RegisterComponent in the constructor).
    for (GameObjectComponent* comp : allComponents)
    {
        comp->OnSpawn();
    }

    // OnSpawn may call AddComponent — those components call OnSpawn themselves
    // immediately (bHasSpawned is already true above).
    OnSpawn();
}

void GameObject::ExecuteTick(float deltaTime)
{
    OnTick(deltaTime);

    // Snapshot so mid-tick component additions don't invalidate the iterator.
    std::vector<GameObjectComponent*> snapshot = tickableComponents;
    for (GameObjectComponent* comp : snapshot)
    {
        comp->OnTick(deltaTime);
    }
}

void GameObject::ExecuteDraw(Renderer& renderer)
{
    OnDraw(renderer);

    std::vector<GameObjectComponent*> snapshot = drawableComponents;
    for (GameObjectComponent* comp : snapshot)
    {
        comp->OnDraw(renderer);
    }
}

void GameObject::ExecuteDestroy()
{
    for (GameObjectComponent* comp : allComponents)
    {
        comp->OnDestroy();
    }
    OnDestroy();
}
