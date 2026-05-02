#include "GameFlowstate.h"

#include "Audio/SoundManager.h"
#include "Core/GameObjectManager.h"
#include "Core/InputManager.h"
#include "Core/GameObjects/SpriteObject.h"
#include "Rendering/Renderer.h"

#include <GLFW/glfw3.h>
#include <imgui.h>

void GameFlowstate::FlowstateEnter()
{
    InputManager::Get().Bind(Input_PlaySound, GLFW_KEY_SPACE);

    testSprite = GameObjectManager::Get().Spawn<SpriteObject>("sprites/test.png");

    startUpSound = SoundManager::Get().LoadSound("sounds/StartUp.wav");
    musicLoop    = SoundManager::Get().LoadMusic("music/MusicLoop.mp3");

    SoundManager::Get().PlayMusic(musicLoop);
}

void GameFlowstate::FlowstateUpdate(float deltaTime)
{
    SoundManager::Get().Update();
    GameObjectManager::Get().Tick(deltaTime);
    GameObjectManager::Get().Draw(Renderer::Get());

    if (InputManager::Get().IsKeyPressed(Input_PlaySound))
    {
        SoundManager::Get().Play(startUpSound);
    }

    ImGui::ShowDemoWindow();
}

void GameFlowstate::FlowstateExit()
{
    SoundManager::Get().StopMusic(musicLoop);

    if (testSprite)
    {
        testSprite->Destroy();
    }
}
