#pragma once

#include "ServiceLocator.h"

#include <unordered_map>

class InputManager
{
public:
    static InputManager& Get() { return ServiceLocator::GetInputManager(); }
    // Associates an arbitrary action value with a GLFW key code.
    // Call this during FlowstateEnter for each action the state needs.
    void Bind(int action, int glfwKey);

    // Polls GLFW for the current state of all bound keys.
    // Must be called once per frame, after glfwPollEvents().
    void Update();

    // True every frame the key is held down.
    bool IsKeyDown(int action) const;

    // True only on the single frame the key transitions from up to down.
    bool IsKeyPressed(int action) const;

    // True only on the single frame the key transitions from down to up.
    bool IsKeyReleased(int action) const;

private:
    struct KeyState
    {
        bool current  = false;
        bool previous = false;
    };

    std::unordered_map<int, int>      bindings;
    std::unordered_map<int, KeyState> keyStates;
};
