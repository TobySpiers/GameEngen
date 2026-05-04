#pragma once

#include <memory>

class AssetManager;
class DebugPanelManager;
class GlobalSettings;
class GameObjectManager;
class InputManager;
class Renderer;
class SoundManager;
struct GLFWwindow;

class ServiceLocator
{
public:
    // Called once in main after the GLFW window is created
    static void ProvideWindow(GLFWwindow* window);

    // Lazily constructs on first access
    static AssetManager&       GetAssetManager();
    static DebugPanelManager&  GetDebugPanelManager();
    static GlobalSettings&     GetGlobalSettings();
    static GameObjectManager&  GetGameObjectManager();
    static InputManager&       GetInputManager();
    static Renderer&           GetRenderer();
    static SoundManager&       GetSoundManager();

    // Returns the GLFW window registered via ProvideWindow
    static GLFWwindow* GetWindow();

    // Destroys all services; call after the flowstate manager is torn down
    static void Shutdown();

private:
    static std::unique_ptr<AssetManager>       s_assetManager;
    static std::unique_ptr<DebugPanelManager>  s_debugPanelManager;
    static std::unique_ptr<GlobalSettings>     s_globalSettings;
    static std::unique_ptr<GameObjectManager>  s_gameObjectManager;
    static std::unique_ptr<InputManager>       s_inputManager;
    static std::unique_ptr<Renderer>           s_renderer;
    static std::unique_ptr<SoundManager>       s_soundManager;
    static GLFWwindow*                         s_window;
};
