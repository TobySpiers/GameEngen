#include "GameObjectComponent.h"
#include "GameObject.h"

#include <algorithm>

void GameObjectComponent::SetTickEnabled(bool bEnabled)
{
    if (bTickEnabled == bEnabled)
    {
        return;
    }
    bTickEnabled = bEnabled;
    if (owner)
    {
        if (bEnabled)
        {
            owner->tickableComponents.push_back(this);
        }
        else
        {
            std::erase(owner->tickableComponents, this);
        }
    }
}

void GameObjectComponent::SetDrawEnabled(bool bEnabled)
{
    if (bDrawEnabled == bEnabled)
    {
        return;
    }
    bDrawEnabled = bEnabled;
    if (owner)
    {
        if (bEnabled)
        {
            owner->drawableComponents.push_back(this);
        }
        else
        {
            std::erase(owner->drawableComponents, this);
        }
    }
}
