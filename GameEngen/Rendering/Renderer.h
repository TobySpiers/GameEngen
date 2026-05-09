#pragma once

#include "UserSettings/GraphicsSettings.h"
#include "ServiceLocator.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <memory>
#include <unordered_map>
#include <vector>

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

    // Draws a thick line between two world-space points with a colour gradient.
    // Thickness is in world units; it is clamped to a minimum of 1 screen pixel.
    void DrawLine(glm::vec2 start,
                  glm::vec2 end,
                  float     thickness,
                  glm::vec4 startColor,
                  glm::vec4 endColor);

    // Draws a filled convex polygon with an optional solid outline.
    // Set fillColor.a or outlineColor.a to 0 to skip that element.
    // Note: fill uses fan triangulation from point 0 — correct for convex polygons.
    void DrawShape(const std::vector<glm::vec2>& points,
                   glm::vec4                     fillColor,
                   glm::vec4                     outlineColor,
                   float                         outlineThickness);

    // Draws a filled convex polygon where each vertex carries its own colour;
    // the interior is shaded by interpolating between them.
    // Note: fill uses fan triangulation from point 0 — correct for convex polygons.
    void DrawGradientShape(const std::vector<glm::vec2>& points,
                           const std::vector<glm::vec4>& colors);

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

    // Per-sprite data uploaded to the GPU as an instanced vertex attribute buffer.
    struct SpriteInstance
    {
        glm::mat4 model;
        glm::vec4 tint;
    };

    // Vertex type shared by all primitive (line/shape) draw calls.
    struct PrimitiveVertex
    {
        glm::vec2 pos;
        glm::vec4 color;
    };

    GLuint                        quadVao        = 0;
    GLuint                        quadVbo        = 0;
    GLuint                        instanceVbo    = 0;
    GLuint                        primitiveVao   = 0;
    GLuint                        primitiveVbo   = 0;
    std::shared_ptr<const Shader> spriteShader;
    std::shared_ptr<const Shader> screenShader;
    std::shared_ptr<const Shader> primitiveShader;
    std::unique_ptr<RenderTarget> renderTarget;
    glm::mat4                     projection     = glm::mat4(1.0f);

    // Sprites queued this frame, grouped by texture ID for batched instanced drawing.
    std::unordered_map<GLuint, std::vector<SpriteInstance>> spriteQueue;

    // All primitive (line/shape) vertices queued this frame, already tessellated
    // into triangles. Flushed in one draw call at the end of the frame.
    std::vector<PrimitiveVertex> primitiveQueue;

    int windowWidth  = 0;
    int windowHeight = 0;

    // Saved windowed state, restored when leaving fullscreen or borderless.
    int savedWindowedX = 0;
    int savedWindowedY = 0;
    int savedWindowedW = 1280;
    int savedWindowedH = 720;

    void InitQuad();
    void InitPrimitiveBuffer();

    // Uploads each texture group's instance data and issues one instanced draw call
    // per unique texture. Clears the queue afterwards.
    void FlushSprites();

    // Uploads the primitive queue to the GPU and issues a single draw call.
    // Clears the queue afterwards.
    void FlushPrimitives();

    // Returns the world-unit size of one screen pixel at the current render
    // target resolution. Used to enforce minimum line/outline thickness.
    float PixelSizeInWorldUnits() const;

    // Returns the monitor that the window most overlaps with.
    struct GLFWmonitor* GetCurrentMonitor() const;
};
