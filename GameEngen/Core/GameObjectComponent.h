#pragma once

class GameObject;
class Renderer;

class GameObjectComponent
{
public:
    virtual ~GameObjectComponent() = default;

    void SetTickEnabled(bool bEnabled);
    void SetDrawEnabled(bool bEnabled);

    bool IsTickEnabled() const { return bTickEnabled; }
    bool IsDrawEnabled() const { return bDrawEnabled; }

    GameObject* GetOwner() const { return owner; }

protected:
    virtual void OnSpawn()                  {}
    virtual void OnTick(float deltaTime)    {}
    virtual void OnDraw(Renderer& renderer) {}
    virtual void OnDestroy()                {}

private:
    friend class GameObject;

    GameObject* owner        = nullptr;
    bool        bTickEnabled = true;
    bool        bDrawEnabled = true;
};
