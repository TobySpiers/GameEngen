#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "ServiceLocator.h"
#include "FlowstateManager.h"
#include "DebugPanels/DebugPanelManager.h"
#include "DebugPanels/DebugPanelAudioSettings.h"
#include "DebugPanels/DebugPanelGraphicsSettings.h"
#include "DebugPanels/DebugPanelImGuiDemo.h"
#include "DebugPanels/DebugPanelLog.h"
#include "DebugPanels/DebugPanelProfiler.h"
#include "Profiler.h"
#include "Flowstates/GameFlowstate.h"
#include "Rendering/Renderer.h"

#include <ctime>
#include <filesystem>
#include <string>

#include "Audio/SoundManager.h"
#include "InputManager.h"
#include "Log.h"
#include "UserSettings/GlobalSettings.h"

static void OnGlfwError(int error, const char* description)
{
    Log::Error(LogCategory::Common, "GLFW error " + std::to_string(error) + ": " + description);
}

int main(int argc, char* argv[])
{
    std::filesystem::current_path(ASSET_DIR);

    {
        std::time_t now = std::time(nullptr);
        std::tm     localTime{};
        localtime_s(&localTime, &now);
        char timeStr[32];
        std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &localTime);
        Log::Info(LogCategory::Common, "Session started " + std::string(timeStr));
    }

    glfwSetErrorCallback(OnGlfwError);

    if (!glfwInit())
    {
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "GameEngen", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        Log::Error(LogCategory::Common, "Failed to initialise GLAD");
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    Log::Info(LogCategory::Graphics, "GPU: "    + std::string(reinterpret_cast<const char*>(glGetString(GL_RENDERER))));
    Log::Info(LogCategory::Graphics, "Vendor: " + std::string(reinterpret_cast<const char*>(glGetString(GL_VENDOR))));
    Log::Info(LogCategory::Graphics, "OpenGL: " + std::string(reinterpret_cast<const char*>(glGetString(GL_VERSION))));
    Log::Info(LogCategory::Graphics, "GLSL: "   + std::string(reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION))));

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    ServiceLocator::ProvideWindow(window);
    {
        DebugPanelManager::Get().RegisterPanel<DebugPanelAudioSettings>();
        DebugPanelManager::Get().RegisterPanel<DebugPanelGraphicsSettings>();
        DebugPanelManager::Get().RegisterPanel<DebugPanelImGuiDemo>();
        DebugPanelManager::Get().RegisterPanel<DebugPanelLog>();
        DebugPanelManager::Get().RegisterPanel<DebugPanelProfiler>();

        GlobalSettings::Get().LogValues();

        FlowstateManager flowstateManager;
        flowstateManager.SwitchTo(std::make_unique<GameFlowstate>());

        double lastTime = glfwGetTime();

        while (!glfwWindowShouldClose(window))
        {
            double now      = glfwGetTime();
            float deltaTime = static_cast<float>(now - lastTime);
            lastTime        = now;

            // Drain any pending GPU commands from the previous frame before
            // opening the profiling window.  Without this, some drivers defer
            // the render-target read/write hazard to the first draw call,
            // inflating a named scope by ~16 ms.  glFinish() here ensures that
            // stall is paid before measurement begins.
            glFinish();
            Profiler::Get().BeginFrame();

            glfwPollEvents();
            InputManager::Get().Update();
            SoundManager::Get().Update();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            int displayWidth, displayHeight;
            glfwGetFramebufferSize(window, &displayWidth, &displayHeight);
            Renderer::Get().BeginFrame(displayWidth, displayHeight);

            flowstateManager.Update(deltaTime);

            Renderer::Get().EndFrame();

            {
                ProfileScope scope("ImGui");
                DebugPanelManager::Get().Update();

                ImGui::Render();
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            }

            Profiler::Get().EndFrame();

            glfwSwapBuffers(window);
        }
    } // FlowstateManager destroyed here; FlowstateExit fires before services shut down

    ServiceLocator::Shutdown();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
