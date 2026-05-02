#include "InputManager.h"
#include "ServiceLocator.h"

#include <GLFW/glfw3.h>

void InputManager::Bind(int action, int glfwKey)
{
    bindings[action]   = glfwKey;
    keyStates[action]  = {};
}

void InputManager::Update()
{
    GLFWwindow* window = ServiceLocator::GetWindow();
    for (auto& [action, key] : bindings)
    {
        KeyState& state   = keyStates[action];
        state.previous    = state.current;
        state.current     = (glfwGetKey(window, key) == GLFW_PRESS);
    }
}

bool InputManager::IsKeyDown(int action) const
{
    auto it = keyStates.find(action);
    if (it == keyStates.end())
    {
        return false;
    }
    return it->second.current;
}

bool InputManager::IsKeyPressed(int action) const
{
    auto it = keyStates.find(action);
    if (it == keyStates.end())
    {
        return false;
    }
    return it->second.current && !it->second.previous;
}

bool InputManager::IsKeyReleased(int action) const
{
    auto it = keyStates.find(action);
    if (it == keyStates.end())
    {
        return false;
    }
    return it->second.previous && !it->second.current;
}
