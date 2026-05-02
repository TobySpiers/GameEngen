#pragma once

class GameObject;
class Renderer;

class GameObjectComponent
{
public:
    virtual ~GameObjectComponent() = default;

    virtual void OnSpawn()                  {}
    virtual void OnTick(float deltaTime)    {}
    virtual void OnDraw(Renderer& renderer) {}
    virtual void OnDestroy()                {}

    void SetTickEnabled(bool bEnabled);
    void SetDrawEnabled(bool bEnabled);

    bool IsTickEnabled() const { return bTickEnabled; }
    bool IsDrawEnabled() const { return bDrawEnabled; }

    GameObject* GetOwner() const { return owner; }

private:
    friend class GameObject;

    GameObject* owner        = nullptr;
    bool        bTickEnabled = true;
    bool        bDrawEnabled = true;
};
