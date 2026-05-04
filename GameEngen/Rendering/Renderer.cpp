#include "Renderer.h"
#include "RenderTarget.h"
#include "Shader.h"
#include "Texture.h"

#include "AssetManager.h"
#include "Log.h"
#include "UserSettings/GraphicsSettings.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <string>

Renderer::Renderer()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    InitQuad();

    spriteShader = AssetManager::Get().GetShader("shaders/sprite.vert", "shaders/sprite.frag");
    screenShader = AssetManager::Get().GetShader("shaders/screen.vert", "shaders/screen.frag");

    renderTarget = std::make_unique<RenderTarget>(
        GraphicsSettings::ResolutionWidths[DefaultResolutionIndex],
        GraphicsSettings::ResolutionHeights[DefaultResolutionIndex]);

    UpdateProjection();

    // Capture initial windowed state so we can restore it later
    GLFWwindow* window = ServiceLocator::GetWindow();
    glfwGetWindowPos(window,  &savedWindowedX, &savedWindowedY);
    glfwGetWindowSize(window, &savedWindowedW, &savedWindowedH);
}

Renderer::~Renderer()
{
    glDeleteVertexArrays(1, &quadVao);
    glDeleteBuffers(1, &quadVbo);
}

void Renderer::BeginFrame(int winWidth, int winHeight)
{
    windowWidth  = winWidth;
    windowHeight = winHeight;

    if (GraphicsSettings::Get().selectedResolution == GraphicsSettings::NativeResolutionIndex)
    {
        if (winWidth != renderTarget->GetWidth() || winHeight != renderTarget->GetHeight())
        {
            renderTarget->Resize(winWidth, winHeight);
            UpdateProjection();
            Log::Info(LogCategory::Graphics, "Resolution: " + std::to_string(winWidth) + "x" + std::to_string(winHeight) + " (native)");
        }
    }

    renderTarget->Bind();

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::EndFrame()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Black background for letterbox/pillarbox bars
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Compute the destination rect that fits the render target into the window
    int drawX, drawY, drawW, drawH;

    if (GraphicsSettings::Get().bStretchToFill)
    {
        drawX = 0;
        drawY = 0;
        drawW = windowWidth;
        drawH = windowHeight;
    }
    else
    {
        const float rtAspect  = static_cast<float>(renderTarget->GetWidth())  /
                                static_cast<float>(renderTarget->GetHeight());
        const float winAspect = static_cast<float>(windowWidth) /
                                static_cast<float>(windowHeight);

        if (rtAspect > winAspect)
        {
            // RT is wider than the window — letterbox (bars top and bottom)
            drawW = windowWidth;
            drawH = static_cast<int>(windowWidth / rtAspect);
            drawX = 0;
            drawY = (windowHeight - drawH) / 2;
        }
        else
        {
            // RT is taller than the window — pillarbox (bars left and right)
            drawH = windowHeight;
            drawW = static_cast<int>(windowHeight * rtAspect);
            drawX = (windowWidth - drawW) / 2;
            drawY = 0;
        }
    }

    glViewport(drawX, drawY, drawW, drawH);

    screenShader->Use();
    screenShader->SetInt("screenTexture", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderTarget->GetTextureId());

    glBindVertexArray(quadVao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

int Renderer::GetRenderTargetWidth() const
{
    return renderTarget->GetWidth();
}

int Renderer::GetRenderTargetHeight() const
{
    return renderTarget->GetHeight();
}

void Renderer::ResizeRenderTarget(int width, int height)
{
    renderTarget->Resize(width, height);
    UpdateProjection();
}

void Renderer::DrawSprite(const Texture& texture, glm::vec2 position, glm::vec2 size,
                          float rotation, glm::vec4 tint)
{
    spriteShader->Use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));

    if (rotation != 0.0f)
    {
        // Rotate around the centre of the sprite
        model = glm::translate(model,  glm::vec3( 0.5f * size,  0.0f));
        model = glm::rotate   (model,  glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::translate(model,  glm::vec3(-0.5f * size,  0.0f));
    }

    model = glm::scale(model, glm::vec3(size, 1.0f));

    spriteShader->SetMat4("model",      model);
    spriteShader->SetMat4("projection", projection);
    spriteShader->SetVec4("tint",       tint);
    spriteShader->SetInt ("tex",        0);

    texture.Bind(0);

    glBindVertexArray(quadVao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Renderer::InitQuad()
{
    // Unit quad spanning (0,0) to (1,1) in local space.
    // (0,0) is bottom-left throughout: position, UV, and screen space all share
    // the same origin convention.
    float vertices[] = {
    //   x      y      u      v
        0.0f,  0.0f,  0.0f,  0.0f,   // bottom-left
        1.0f,  0.0f,  1.0f,  0.0f,   // bottom-right
        0.0f,  1.0f,  0.0f,  1.0f,   // top-left

        1.0f,  0.0f,  1.0f,  0.0f,   // bottom-right
        1.0f,  1.0f,  1.0f,  1.0f,   // top-right
        0.0f,  1.0f,  0.0f,  1.0f,   // top-left
    };

    glGenVertexArrays(1, &quadVao);
    glGenBuffers(1, &quadVbo);

    glBindVertexArray(quadVao);

    glBindBuffer(GL_ARRAY_BUFFER, quadVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position — location 0
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // UV — location 1
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Renderer::SetWindowMode(WindowMode mode)
{
    GLFWwindow* window = ServiceLocator::GetWindow();

    // Leaving windowed — save current position and size for later restoration
    if (GraphicsSettings::Get().windowMode == WindowMode::Windowed)
    {
        glfwGetWindowPos(window,  &savedWindowedX, &savedWindowedY);
        glfwGetWindowSize(window, &savedWindowedW, &savedWindowedH);
    }

    GraphicsSettings::Get().windowMode = mode;

    switch (mode)
    {
        case WindowMode::Windowed:
        {
            glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_TRUE);
            glfwSetWindowMonitor(window, nullptr,
                                 savedWindowedX, savedWindowedY,
                                 savedWindowedW, savedWindowedH, 0);
            break;
        }
        case WindowMode::BorderlessWindowed:
        {
            GLFWmonitor*       monitor = GetCurrentMonitor();
            const GLFWvidmode* vidMode = glfwGetVideoMode(monitor);
            int mx, my;
            glfwGetMonitorPos(monitor, &mx, &my);

            glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE);
            glfwSetWindowMonitor(window, nullptr, mx, my, vidMode->width, vidMode->height, 0);
            break;
        }
        case WindowMode::Fullscreen:
        {
            GLFWmonitor*       monitor = GetCurrentMonitor();
            const GLFWvidmode* vidMode = glfwGetVideoMode(monitor);

            glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_TRUE);
            glfwSetWindowMonitor(window, monitor, 0, 0,
                                 vidMode->width, vidMode->height, vidMode->refreshRate);
            break;
        }
    }
}

GLFWmonitor* Renderer::GetCurrentMonitor() const
{
    GLFWwindow* window = ServiceLocator::GetWindow();

    int wx, wy, ww, wh;
    glfwGetWindowPos(window,  &wx, &wy);
    glfwGetWindowSize(window, &ww, &wh);

    int           monitorCount = 0;
    GLFWmonitor** monitors     = glfwGetMonitors(&monitorCount);
    GLFWmonitor*  bestMonitor  = glfwGetPrimaryMonitor();
    int           bestOverlap  = 0;

    for (int i = 0; i < monitorCount; ++i)
    {
        int mx, my;
        glfwGetMonitorPos(monitors[i], &mx, &my);
        const GLFWvidmode* vidMode = glfwGetVideoMode(monitors[i]);

        const int overlapW = std::max(0, std::min(wx + ww, mx + vidMode->width)  - std::max(wx, mx));
        const int overlapH = std::max(0, std::min(wy + wh, my + vidMode->height) - std::max(wy, my));
        const int overlap  = overlapW * overlapH;

        if (overlap > bestOverlap)
        {
            bestOverlap = overlap;
            bestMonitor = monitors[i];
        }
    }

    return bestMonitor;
}

void Renderer::UpdateProjection()
{
    const float aspect    = static_cast<float>(renderTarget->GetWidth()) /
                            static_cast<float>(renderTarget->GetHeight());
    const float worldHigh = GraphicsSettings::Get().worldUnitsWide / aspect;
    projection = glm::ortho(0.0f, GraphicsSettings::Get().worldUnitsWide, 0.0f, worldHigh, -1.0f, 1.0f);
}
