#pragma once

#include "GameObject.h"
#include "ServiceLocator.h"

#include <memory>
#include <type_traits>
#include <vector>

class Renderer;

class GameObjectManager
{
public:
    static GameObjectManager& Get() { return ServiceLocator::GetGameObjectManager(); }
    // Constructs a GameObject of type T, registers it, and calls OnSpawn.
    // Returns a non-owning pointer for the caller's convenience.
    template<typename T, typename... Args>
    T* Spawn(Args&&... args);

    // Ticks all tick-enabled objects, then flushes the destroy queue.
    void Tick(float deltaTime);

    // Draws all draw-enabled objects, then flushes the destroy queue.
    void Draw(Renderer& renderer);

    // Called by GameObject::SetTickEnabled / SetDrawEnabled
    void OnTickEnabledChanged(GameObject* obj, bool bEnabled);
    void OnDrawEnabledChanged(GameObject* obj, bool bEnabled);

    // Called by GameObject::Destroy — adds obj to the pending destroy queue.
    void RequestDestroy(GameObject* obj);

private:
    std::vector<std::unique_ptr<GameObject>> allObjects;
    std::vector<GameObject*>                 tickableObjects;
    std::vector<GameObject*>                 drawableObjects;
    std::vector<GameObject*>                 pendingDestroyQueue;

    // Calls OnDestroy on all pending objects and removes them from all lists.
    void FlushDestroyQueue();
};

template<typename T, typename... Args>
T* GameObjectManager::Spawn(Args&&... args)
{
    static_assert(std::is_base_of_v<GameObject, T>, "T must derive from GameObject");

    auto obj = std::make_unique<T>(std::forward<Args>(args)...);
    T* ptr   = obj.get();

    // Ownership transferred before OnSpawn fires, so child spawns during
    // OnSpawn are safe — the parent is already tracked.
    allObjects.push_back(std::move(obj));

    if (ptr->IsTickEnabled())
    {
        tickableObjects.push_back(ptr);
    }
    if (ptr->IsDrawEnabled())
    {
        drawableObjects.push_back(ptr);
    }

    ptr->ExecuteSpawn();

    return ptr;
}
