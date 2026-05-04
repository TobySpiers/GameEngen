#pragma once

#include "GameObjectComponent.h"
#include "Transform.h"

#include <memory>
#include <type_traits>
#include <vector>

class Renderer;

class GameObject
{
public:
    virtual ~GameObject() = default;

    virtual void OnSpawn()                  {}
    virtual void OnTick(float deltaTime)    {}
    virtual void OnDraw(Renderer& renderer) {}
    virtual void OnDestroy()                {}

    void SetTickEnabled(bool bEnabled);
    void SetDrawEnabled(bool bEnabled);

    // Marks this object for destruction at the end of the current tick.
    void Destroy();

    bool IsTickEnabled()    const { return bTickEnabled; }
    bool IsDrawEnabled()    const { return bDrawEnabled; }
    bool IsPendingDestroy() const { return bPendingDestroy; }

    // Constructs a component of type T, takes ownership, and registers it for dispatch.
    // Returns a non-owning pointer for the caller's convenience.
    template<typename T, typename... Args>
    T* AddComponent(Args&&... args);

    // Returns the first component of type T, or nullptr if none found.
    template<typename T>
    T* GetComponent();

    Transform transform;

private:
    friend class GameObjectManager;
    friend class GameObjectComponent;

    // Called by GameObjectManager — dispatches to the object and all registered components.
    void ExecuteSpawn();
    void ExecuteTick(float deltaTime);
    void ExecuteDraw(Renderer& renderer);
    void ExecuteDestroy();

    bool bTickEnabled    = true;
    bool bDrawEnabled    = true;
    bool bPendingDestroy = false;
    bool bHasSpawned     = false;

    std::vector<std::unique_ptr<GameObjectComponent>> ownedComponents;
    std::vector<GameObjectComponent*>                 allComponents;
    std::vector<GameObjectComponent*>                 tickableComponents;
    std::vector<GameObjectComponent*>                 drawableComponents;
};

template<typename T, typename... Args>
T* GameObject::AddComponent(Args&&... args)
{
    static_assert(std::is_base_of_v<GameObjectComponent, T>, "T must derive from GameObjectComponent");

    auto comp = std::make_unique<T>(std::forward<Args>(args)...);
    T* ptr    = comp.get();

    ptr->owner = this;

    allComponents.push_back(ptr);

    if (ptr->IsTickEnabled())
    {
        tickableComponents.push_back(ptr);
    }
    if (ptr->IsDrawEnabled())
    {
        drawableComponents.push_back(ptr);
    }

    // Ownership transferred before OnSpawn fires so the component is already
    // tracked if it spawns child components of its own.
    ownedComponents.push_back(std::move(comp));

    // If the owning object has already spawned, fire OnSpawn immediately.
    // Cast to GameObjectComponent* so the call goes through the base-class
    // access specifier, where GameObject is a friend.
    if (bHasSpawned)
    {
        static_cast<GameObjectComponent*>(ptr)->OnSpawn();
    }

    return ptr;
}

template<typename T>
T* GameObject::GetComponent()
{
    for (GameObjectComponent* comp : allComponents)
    {
        if (T* casted = dynamic_cast<T*>(comp))
        {
            return casted;
        }
    }
    return nullptr;
}
