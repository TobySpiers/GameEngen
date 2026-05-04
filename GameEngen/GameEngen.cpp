#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "ServiceLocator.h"
#include "FlowstateManager.h"
#include "Debug/DebugPanelManager.h"
#include "Debug/DebugPanelGraphicsSettings.h"
#include "Debug/DebugPanelImGuiDemo.h"
#include "Flowstates/GameFlowstate.h"
#include "Rendering/Renderer.h"

#include <cstdio>
#include <filesystem>

#include "InputManager.h"

static void OnGlfwError(int error, const char* description)
{
    fprintf(stderr, "GLFW error %d: %s\n", error, description);
}

int main(int argc, char* argv[])
{
    std::filesystem::current_path(ASSET_DIR);

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
        fprintf(stderr, "Failed to initialise GLAD\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    ServiceLocator::ProvideWindow(window);
    {
        DebugPanelManager::Get().RegisterPanel<DebugPanelGraphicsSettings>();
        DebugPanelManager::Get().RegisterPanel<DebugPanelImGuiDemo>();

        FlowstateManager flowstateManager;
        flowstateManager.SwitchTo(std::make_unique<GameFlowstate>());

        double lastTime = glfwGetTime();

        while (!glfwWindowShouldClose(window))
        {
            double now      = glfwGetTime();
            float deltaTime = static_cast<float>(now - lastTime);
            lastTime        = now;

            glfwPollEvents();
            InputManager::Get().Update();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            int displayWidth, displayHeight;
            glfwGetFramebufferSize(window, &displayWidth, &displayHeight);
            Renderer::Get().BeginFrame(displayWidth, displayHeight);

            flowstateManager.Update(deltaTime);

            Renderer::Get().EndFrame();

            DebugPanelManager::Get().Update();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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
