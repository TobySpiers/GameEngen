#include "ServiceLocator.h"

#include "AssetManager.h"
#include "Debug/DebugPanelManager.h"
#include "UserSettings/GraphicsSettings.h"
#include "GameObjectManager.h"
#include "InputManager.h"
#include "Rendering/Renderer.h"
#include "Audio/SoundManager.h"

#include <cstdio>

// Static member definitions
std::unique_ptr<AssetManager>      ServiceLocator::s_assetManager;
std::unique_ptr<DebugPanelManager> ServiceLocator::s_debugPanelManager;
std::unique_ptr<GraphicsSettings>  ServiceLocator::s_graphicsSettings;
std::unique_ptr<GameObjectManager> ServiceLocator::s_gameObjectManager;
std::unique_ptr<InputManager>      ServiceLocator::s_inputManager;
std::unique_ptr<Renderer>          ServiceLocator::s_renderer;
std::unique_ptr<SoundManager>      ServiceLocator::s_soundManager;
GLFWwindow*                        ServiceLocator::s_window = nullptr;

AssetManager& ServiceLocator::GetAssetManager()
{
    if (!s_assetManager)
    {
        s_assetManager = std::make_unique<AssetManager>();
    }
    return *s_assetManager;
}

DebugPanelManager& ServiceLocator::GetDebugPanelManager()
{
    if (!s_debugPanelManager)
    {
        s_debugPanelManager = std::make_unique<DebugPanelManager>();
    }
    return *s_debugPanelManager;
}

GraphicsSettings& ServiceLocator::GetGraphicsSettings()
{
    if (!s_graphicsSettings)
    {
        s_graphicsSettings = std::make_unique<GraphicsSettings>();
    }
    return *s_graphicsSettings;
}

void ServiceLocator::ProvideWindow(GLFWwindow* window)
{
    s_window = window;
}

GLFWwindow* ServiceLocator::GetWindow()
{
    if (!s_window)
    {
        fprintf(stderr, "ServiceLocator: GetWindow() called before ProvideWindow()\n");
    }
    return s_window;
}

GameObjectManager& ServiceLocator::GetGameObjectManager()
{
    if (!s_gameObjectManager)
    {
        s_gameObjectManager = std::make_unique<GameObjectManager>();
    }
    return *s_gameObjectManager;
}

InputManager& ServiceLocator::GetInputManager()
{
    if (!s_inputManager)
    {
        s_inputManager = std::make_unique<InputManager>();
    }
    return *s_inputManager;
}

Renderer& ServiceLocator::GetRenderer()
{
    if (!s_renderer)
    {
        s_renderer = std::make_unique<Renderer>();
    }
    return *s_renderer;
}

SoundManager& ServiceLocator::GetSoundManager()
{
    if (!s_soundManager)
    {
        s_soundManager = std::make_unique<SoundManager>();
    }
    return *s_soundManager;
}

void ServiceLocator::Shutdown()
{
    s_assetManager.reset();
    s_debugPanelManager.reset();
    s_graphicsSettings.reset();
    s_gameObjectManager.reset();
    s_inputManager.reset();
    s_soundManager.reset();
    s_renderer.reset();
    s_window = nullptr;
}
