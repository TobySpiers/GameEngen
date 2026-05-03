#include "GameFlowstate.h"

#include "Audio/SoundManager.h"
#include "Core/GameObjectManager.h"
#include "Core/InputManager.h"
#include "Core/GameObjects/PlayerObject.h"
#include "Rendering/Renderer.h"

#include <GLFW/glfw3.h>

void GameFlowstate::FlowstateEnter()
{
    InputManager::Get().Bind(Input_PlaySound, GLFW_KEY_SPACE);
    InputManager::Get().Bind(Input_MoveLeft,  GLFW_KEY_A);
    InputManager::Get().Bind(Input_MoveRight, GLFW_KEY_D);
    InputManager::Get().Bind(Input_MoveDown,  GLFW_KEY_S);
    InputManager::Get().Bind(Input_MoveUp,    GLFW_KEY_W);

    player = GameObjectManager::Get().Spawn<PlayerObject>("sprites/test.png");

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
}

void GameFlowstate::FlowstateExit()
{
    SoundManager::Get().StopMusic(musicLoop);

    if (player)
    {
        player->Destroy();
    }
}
