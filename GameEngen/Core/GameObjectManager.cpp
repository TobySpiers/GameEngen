#include "GameObjectManager.h"
#include "Rendering/Renderer.h"

#include <algorithm>

void GameObjectManager::Tick(float deltaTime)
{
    // Snapshot so mid-tick list changes (SetTickEnabled, Destroy) don't
    // invalidate the iterator.
    std::vector<GameObject*> snapshot = tickableObjects;
    for (GameObject* obj : snapshot)
    {
        if (!obj->IsPendingDestroy())
        {
            obj->ExecuteTick(deltaTime);
        }
    }
    FlushDestroyQueue();
}

void GameObjectManager::Draw(Renderer& renderer)
{
    std::vector<GameObject*> snapshot = drawableObjects;
    for (GameObject* obj : snapshot)
    {
        if (!obj->IsPendingDestroy())
        {
            obj->ExecuteDraw(renderer);
        }
    }
    FlushDestroyQueue();
}

void GameObjectManager::OnTickEnabledChanged(GameObject* obj, bool bEnabled)
{
    if (bEnabled)
    {
        tickableObjects.push_back(obj);
    }
    else
    {
        std::erase(tickableObjects, obj);
    }
}

void GameObjectManager::OnDrawEnabledChanged(GameObject* obj, bool bEnabled)
{
    if (bEnabled)
    {
        drawableObjects.push_back(obj);
    }
    else
    {
        std::erase(drawableObjects, obj);
    }
}

void GameObjectManager::RequestDestroy(GameObject* obj)
{
    pendingDestroyQueue.push_back(obj);
}

void GameObjectManager::FlushDestroyQueue()
{
    for (GameObject* obj : pendingDestroyQueue)
    {
        obj->ExecuteDestroy();
    }

    auto isPending = [](const GameObject* obj) { return obj->IsPendingDestroy(); };

    std::erase_if(tickableObjects, isPending);
    std::erase_if(drawableObjects, isPending);
    std::erase_if(allObjects, [](const std::unique_ptr<GameObject>& p) { return p->IsPendingDestroy(); });

    pendingDestroyQueue.clear();
}

