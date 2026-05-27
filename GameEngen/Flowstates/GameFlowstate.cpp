#include "GameFlowstate.h"

#include "AssetManager.h"
#include "Audio/SoundManager.h"
#include "GameObjectManager.h"
#include "InputManager.h"
#include "GameObjects/PlayerObject.h"
#include "GameObjects/StaticMeshObject.h"
#include "Rendering/MeshAsset.h"
#include "Rendering/Renderer.h"

#include <GLFW/glfw3.h>
#include <cmath>

void GameFlowstate::FlowstateEnter()
{
    InputManager::Get().Bind(Input_PlaySound, GLFW_KEY_SPACE);
    InputManager::Get().Bind(Input_MoveLeft,  GLFW_KEY_A);
    InputManager::Get().Bind(Input_MoveRight, GLFW_KEY_D);
    InputManager::Get().Bind(Input_MoveDown,  GLFW_KEY_S);
    InputManager::Get().Bind(Input_MoveUp,    GLFW_KEY_W);

    player = GameObjectManager::Get().Spawn<PlayerObject>();

    musicLoop = SoundManager::Get().LoadMusic("music/MusicLoop.mp3");
    SoundManager::Get().PlayMusic(musicLoop);

    auto cubeAsset = MeshAsset::CreateCube();
    AssetManager::Get().RegisterMeshAsset("cube", cubeAsset);

    Renderer::Get().Set3DCamera(glm::vec3(0.0f, 2.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f));

    cubeObj = GameObjectManager::Get().Spawn<StaticMeshObject>(cubeAsset);
    cubeObj->transform.position = glm::vec3(-1.0f, 0.0f, 0.0f);

    cubeObj2 = GameObjectManager::Get().Spawn<StaticMeshObject>(cubeAsset, "sprites/test.png");
    cubeObj2->transform.position = glm::vec3(1.0f, 0.0f, 0.0f);
}

void GameFlowstate::FlowstateUpdate(float deltaTime)
{
    time += deltaTime;

    if (cubeObj)
    {
        cubeObj->transform.rotation.y = time * 45.0f;
        cubeObj->transform.position.y = std::sin(time * 1.5f) * 0.5f;
    }

    if (cubeObj2)
    {
        cubeObj2->transform.rotation.y = time * 45.0f;
        cubeObj2->transform.position.y = std::sin(time * 1.5f + 3.14159265f) * 0.5f;
    }

    GameObjectManager::Get().Tick(deltaTime);
    GameObjectManager::Get().Draw(Renderer::Get());

    Renderer& renderer = Renderer::Get();

    // -----------------------------------------------------------------------
    // Lines
    // -----------------------------------------------------------------------

    // Thin horizontal gradient: yellow → cyan
    renderer.DrawLine(
        glm::vec2(  50.0f, 690.0f), glm::vec2(550.0f, 690.0f),
        2.0f,
        glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),
        glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));

    // Thick diagonal gradient: red → blue
    renderer.DrawLine(
        glm::vec2(  50.0f,  50.0f), glm::vec2(420.0f, 420.0f),
        8.0f,
        glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
        glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

    // Vertical line fading to transparent (white → invisible)
    renderer.DrawLine(
        glm::vec2(500.0f,  50.0f), glm::vec2(500.0f, 670.0f),
        2.0f,
        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
        glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));

    // -----------------------------------------------------------------------
    // Shapes
    // -----------------------------------------------------------------------

    // Rectangle: semi-transparent green fill, solid white outline
    renderer.DrawShape(
        {
            glm::vec2(580.0f, 380.0f),
            glm::vec2(780.0f, 380.0f),
            glm::vec2(780.0f, 580.0f),
            glm::vec2(580.0f, 580.0f),
        },
        glm::vec4(0.2f, 0.8f, 0.2f, 0.5f),
        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
        3.0f);

    // Triangle: solid blue fill, no outline
    renderer.DrawShape(
        {
            glm::vec2( 840.0f,  80.0f),
            glm::vec2(1080.0f,  80.0f),
            glm::vec2( 960.0f, 340.0f),
        },
        glm::vec4(0.2f, 0.4f, 1.0f, 1.0f),
        glm::vec4(0.0f),
        0.0f);

    // Diamond: thick orange outline only, no fill
    renderer.DrawShape(
        {
            glm::vec2(640.0f, 390.0f),
            glm::vec2(790.0f, 540.0f),
            glm::vec2(640.0f, 690.0f),
            glm::vec2(490.0f, 540.0f),
        },
        glm::vec4(0.0f),
        glm::vec4(1.0f, 0.5f, 0.0f, 1.0f),
        5.0f);

    // -----------------------------------------------------------------------
    // Gradient shapes
    // -----------------------------------------------------------------------

    // Triangle: red, green, blue at each vertex
    renderer.DrawGradientShape(
        {
            glm::vec2( 880.0f, 380.0f),
            glm::vec2(1180.0f, 380.0f),
            glm::vec2(1030.0f, 670.0f),
        },
        {
            glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
            glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
            glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
        });

    // Quad: a different vivid colour at each corner
    renderer.DrawGradientShape(
        {
            glm::vec2( 880.0f,  50.0f),
            glm::vec2(1180.0f,  50.0f),
            glm::vec2(1180.0f, 330.0f),
            glm::vec2( 880.0f, 330.0f),
        },
        {
            glm::vec4(1.0f, 0.0f, 1.0f, 1.0f),   // magenta
            glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),   // yellow
            glm::vec4(0.0f, 1.0f, 1.0f, 1.0f),   // cyan
            glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),   // white
        });
}

void GameFlowstate::FlowstateExit()
{
    SoundManager::Get().StopMusic(musicLoop);

    if (player)
    {
        player->Destroy();
    }

    if (cubeObj)
    {
        cubeObj->Destroy();
    }

    if (cubeObj2)
    {
        cubeObj2->Destroy();
    }

    AssetManager::Get().UnloadAllMeshAssets();
}
