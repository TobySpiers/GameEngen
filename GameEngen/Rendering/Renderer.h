#pragma once

#include "UserSettings/GraphicsSettings.h"
#include "ServiceLocator.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <memory>

class RenderTarget;
class Shader;
class Texture;

class Renderer
{
public:
    static Renderer& Get() { return ServiceLocator::GetRenderer(); }

public:
    Renderer();
    ~Renderer();

    Renderer(const Renderer&)            = delete;
    Renderer& operator=(const Renderer&) = delete;

    // Binds the render target and clears it, ready for game draw calls.
    void BeginFrame(int winWidth, int winHeight);

    // Blits the render target to the default framebuffer, letterboxing or
    // pillarboxing as needed to preserve the render target's aspect ratio.
    void EndFrame();

    // Draws a textured quad at the given position and size (in world units),
    // with optional rotation in degrees and tint colour.
    void DrawSprite(const Texture&  texture,
                    glm::vec2       position,
                    glm::vec2       size,
                    float           rotation = 0.0f,
                    glm::vec4       tint     = glm::vec4(1.0f));

    int GetWindowWidth()        const { return windowWidth; }
    int GetWindowHeight()       const { return windowHeight; }
    int GetRenderTargetWidth()  const;
    int GetRenderTargetHeight() const;

    // Resizes the render target and updates the orthographic projection to match.
    void ResizeRenderTarget(int width, int height);

    // Recomputes the orthographic projection from GraphicsSettings::worldUnitsWide
    // and the render target's current aspect ratio.
    void UpdateProjection();

    // Applies the given window mode via GLFW.
    void SetWindowMode(WindowMode mode);

private:
    static constexpr int DefaultResolutionIndex = 1; // 1280x720

    GLuint                        quadVao    = 0;
    GLuint                        quadVbo    = 0;
    std::shared_ptr<const Shader> spriteShader;
    std::shared_ptr<const Shader> screenShader;
    std::unique_ptr<RenderTarget> renderTarget;
    glm::mat4                     projection = glm::mat4(1.0f);

    int windowWidth  = 0;
    int windowHeight = 0;

    // Saved windowed state, restored when leaving fullscreen or borderless.
    int savedWindowedX = 0;
    int savedWindowedY = 0;
    int savedWindowedW = 1280;
    int savedWindowedH = 720;

    void InitQuad();

    // Returns the monitor that the window most overlaps with.
    struct GLFWmonitor* GetCurrentMonitor() const;
};
