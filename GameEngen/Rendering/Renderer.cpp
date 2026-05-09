#include "Renderer.h"
#include "RenderTarget.h"
#include "Shader.h"
#include "Texture.h"

#include "AssetManager.h"
#include "Log.h"
#include "Profiler.h"
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
    InitPrimitiveBuffer();

    spriteShader    = AssetManager::Get().GetShader("shaders/sprite.vert",     "shaders/sprite.frag");
    screenShader    = AssetManager::Get().GetShader("shaders/screen.vert",     "shaders/screen.frag");
    primitiveShader = AssetManager::Get().GetShader("shaders/primitive.vert",  "shaders/primitive.frag");

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
    glDeleteBuffers(1, &instanceVbo);
    glDeleteVertexArrays(1, &primitiveVao);
    glDeleteBuffers(1, &primitiveVbo);
}

void Renderer::BeginFrame(int winWidth, int winHeight)
{
    ProfileScope scope("Renderer");

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
    ProfileScope scope("Renderer");

    FlushSprites();
    FlushPrimitives();

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

    spriteQueue[texture.GetId()].push_back({ model, tint });
}

void Renderer::FlushSprites()
{
    if (spriteQueue.empty())
    {
        return;
    }

    spriteShader->Use();
    spriteShader->SetMat4("projection", projection);
    spriteShader->SetInt ("tex",        0);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(quadVao);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);

    for (auto& [textureId, instances] : spriteQueue)
    {
        if (instances.empty())
        {
            continue;
        }

        glBufferData(GL_ARRAY_BUFFER,
                     static_cast<GLsizeiptr>(instances.size() * sizeof(SpriteInstance)),
                     instances.data(),
                     GL_STREAM_DRAW);

        glBindTexture(GL_TEXTURE_2D, textureId);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, static_cast<GLsizei>(instances.size()));

        instances.clear();
    }

    glBindVertexArray(0);
}

float Renderer::PixelSizeInWorldUnits() const
{
    return GraphicsSettings::Get().worldUnitsWide /
           static_cast<float>(renderTarget->GetWidth());
}

// ---------------------------------------------------------------------------
// Primitive drawing
// ---------------------------------------------------------------------------

void Renderer::DrawLine(glm::vec2 start, glm::vec2 end,
                        float thickness,
                        glm::vec4 startColor, glm::vec4 endColor)
{
    const glm::vec2 delta = end - start;
    const float     len   = glm::length(delta);
    if (len < 1e-6f)
    {
        return;
    }

    // Ensure the line is at least 1 screen pixel wide regardless of orientation.
    const float clampedThickness = std::max(thickness, PixelSizeInWorldUnits());

    const glm::vec2 dir  = delta / len;
    const glm::vec2 perp = glm::vec2(-dir.y, dir.x) * (clampedThickness * 0.5f);

    //  v1 -------- v2
    //  |            |
    //  v0 -------- v3
    const PrimitiveVertex v0 { start - perp, startColor };
    const PrimitiveVertex v1 { start + perp, startColor };
    const PrimitiveVertex v2 { end   + perp, endColor   };
    const PrimitiveVertex v3 { end   - perp, endColor   };

    primitiveQueue.push_back(v0);
    primitiveQueue.push_back(v1);
    primitiveQueue.push_back(v2);
    primitiveQueue.push_back(v0);
    primitiveQueue.push_back(v2);
    primitiveQueue.push_back(v3);
}

void Renderer::DrawShape(const std::vector<glm::vec2>& points,
                         glm::vec4 fillColor, glm::vec4 outlineColor,
                         float outlineThickness)
{
    const int n = static_cast<int>(points.size());
    if (n < 3)
    {
        return;
    }

    // --- Fill (fan triangulation from vertex 0) ---
    if (fillColor.a > 0.0f)
    {
        for (int i = 1; i < n - 1; ++i)
        {
            primitiveQueue.push_back({ points[0],         fillColor });
            primitiveQueue.push_back({ points[i],         fillColor });
            primitiveQueue.push_back({ points[i + 1],     fillColor });
        }
    }

    // --- Outline (miter-joint quad strip) ---
    if (outlineColor.a > 0.0f && outlineThickness > 0.0f)
    {
        const float clampedThickness = std::max(outlineThickness, PixelSizeInWorldUnits());
        const float halfT = clampedThickness * 0.5f;

        // Determine polygon winding from the signed area.
        // Positive = CCW (y-up), negative = CW.
        float signedArea = 0.0f;
        for (int i = 0; i < n; ++i)
        {
            const int j = (i + 1) % n;
            signedArea += points[i].x * points[j].y - points[j].x * points[i].y;
        }
        const float normalSign = (signedArea >= 0.0f) ? 1.0f : -1.0f;

        // Compute outer/inner miter vertices for each polygon vertex.
        std::vector<glm::vec2> outerVerts(n);
        std::vector<glm::vec2> innerVerts(n);

        for (int i = 0; i < n; ++i)
        {
            const int prevIdx = (i - 1 + n) % n;
            const int nextIdx = (i + 1) % n;

            const glm::vec2 toPrev = points[i] - points[prevIdx];
            const glm::vec2 toNext = points[nextIdx] - points[i];

            const glm::vec2 prevDir = glm::length(toPrev) > 1e-6f
                                    ? glm::normalize(toPrev)
                                    : glm::vec2(1.0f, 0.0f);
            const glm::vec2 nextDir = glm::length(toNext) > 1e-6f
                                    ? glm::normalize(toNext)
                                    : glm::vec2(1.0f, 0.0f);

            // Outward edge normals (perpendicular, pointing away from polygon interior).
            const glm::vec2 prevNorm = normalSign * glm::vec2( prevDir.y, -prevDir.x);
            const glm::vec2 nextNorm = normalSign * glm::vec2( nextDir.y, -nextDir.x);

            const glm::vec2 miterSum = prevNorm + nextNorm;
            glm::vec2 miterDir;
            float     miterDot;

            if (glm::length(miterSum) < 1e-6f)
            {
                // Antiparallel normals (cusp) — fall back to the arriving edge normal.
                miterDir = prevNorm;
                miterDot = 1.0f;
            }
            else
            {
                miterDir = glm::normalize(miterSum);
                miterDot = std::max(glm::dot(miterDir, prevNorm), 0.1f);
            }

            // Clamp to 10× half-thickness to prevent extreme spikes at sharp angles.
            const float miterLength = std::min(halfT / miterDot, halfT * 10.0f);

            outerVerts[i] = points[i] + miterDir * miterLength;
            innerVerts[i] = points[i] - miterDir * miterLength;
        }

        // Emit one quad per edge.
        for (int i = 0; i < n; ++i)
        {
            const int j = (i + 1) % n;

            const PrimitiveVertex outerA { outerVerts[i], outlineColor };
            const PrimitiveVertex innerA { innerVerts[i], outlineColor };
            const PrimitiveVertex outerB { outerVerts[j], outlineColor };
            const PrimitiveVertex innerB { innerVerts[j], outlineColor };

            primitiveQueue.push_back(outerA);
            primitiveQueue.push_back(innerA);
            primitiveQueue.push_back(innerB);
            primitiveQueue.push_back(outerA);
            primitiveQueue.push_back(innerB);
            primitiveQueue.push_back(outerB);
        }
    }
}

void Renderer::DrawGradientShape(const std::vector<glm::vec2>& points,
                                 const std::vector<glm::vec4>& colors)
{
    const int n = static_cast<int>(points.size());
    if (n < 3 || static_cast<int>(colors.size()) != n)
    {
        return;
    }

    // Fan triangulation from vertex 0 with per-vertex colours.
    for (int i = 1; i < n - 1; ++i)
    {
        primitiveQueue.push_back({ points[0],     colors[0]     });
        primitiveQueue.push_back({ points[i],     colors[i]     });
        primitiveQueue.push_back({ points[i + 1], colors[i + 1] });
    }
}

void Renderer::FlushPrimitives()
{
    if (primitiveQueue.empty())
    {
        return;
    }

    primitiveShader->Use();
    primitiveShader->SetMat4("projection", projection);

    glBindVertexArray(primitiveVao);
    glBindBuffer(GL_ARRAY_BUFFER, primitiveVbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(primitiveQueue.size() * sizeof(PrimitiveVertex)),
                 primitiveQueue.data(),
                 GL_STREAM_DRAW);

    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(primitiveQueue.size()));

    primitiveQueue.clear();
    glBindVertexArray(0);
}

void Renderer::InitPrimitiveBuffer()
{
    glGenVertexArrays(1, &primitiveVao);
    glGenBuffers(1, &primitiveVbo);

    glBindVertexArray(primitiveVao);
    glBindBuffer(GL_ARRAY_BUFFER, primitiveVbo);

    // Position: vec2 at location 0, offset 0
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(PrimitiveVertex), (void*)0);
    glEnableVertexAttribArray(0);

    // Color: vec4 at location 1, offset sizeof(vec2) = 8 bytes
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(PrimitiveVertex),
                          (void*)(sizeof(glm::vec2)));
    glEnableVertexAttribArray(1);

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
    glGenBuffers(1, &instanceVbo);

    glBindVertexArray(quadVao);

    // Per-vertex data: position (location 0) and UV (location 1)
    glBindBuffer(GL_ARRAY_BUFFER, quadVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Per-instance data: model matrix as 4 × vec4 (locations 2–5), tint vec4 (location 6)
    glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);

    for (int i = 0; i < 4; ++i)
    {
        const GLuint attribLoc = 2 + static_cast<GLuint>(i);
        glEnableVertexAttribArray(attribLoc);
        glVertexAttribPointer(attribLoc, 4, GL_FLOAT, GL_FALSE, sizeof(SpriteInstance),
                              reinterpret_cast<void*>(i * sizeof(glm::vec4)));
        glVertexAttribDivisor(attribLoc, 1);
    }

    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(SpriteInstance),
                          reinterpret_cast<void*>(sizeof(glm::mat4)));
    glVertexAttribDivisor(6, 1);

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
