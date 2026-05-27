#pragma once

#include "UserSettings/GraphicsSettings.h"
#include "ServiceLocator.h"
#include "Transform.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Mesh;
class MeshAsset;
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
    // with optional rotation in degrees, tint colour, and depth layer.
    // Depth range is [-100, 100]: more negative = closer to camera (renders on
    // top); more positive = further away (renders behind). Default is 0.
    // Note: this range is determined by the near/far planes of the orthographic
    // projection in UpdateProjection — sprites outside it will be clipped.
    void DrawSprite(const Texture&  texture,
                    glm::vec2       position,
                    glm::vec2       size,
                    float           rotation = 0.0f,
                    glm::vec4       tint     = glm::vec4(1.0f),
                    float           depth    = 0.0f);

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

    // Queues a mesh for instanced drawing with the given transform (in world space).
    // Rotation is Euler angles in degrees applied Y → X → Z.
    void DrawMesh(const Mesh&      mesh,
                  const Transform& transform,
                  glm::vec4        tint = glm::vec4(1.0f));

    void DrawMesh(const Mesh&  mesh,
                  glm::vec3    position,
                  glm::vec3    rotation = glm::vec3(0.0f),
                  glm::vec3    scale    = glm::vec3(1.0f),
                  glm::vec4    tint     = glm::vec4(1.0f));

    // Updates the view and perspective projection matrices used by DrawMesh.
    // Call once per frame before any DrawMesh calls if the camera has moved.
    void Set3DCamera(glm::vec3 position,
                     glm::vec3 target,
                     float     fovDegrees = 60.0f);

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

    // Returns the world-unit size of one screen pixel at the current render
    // target resolution. Useful for specifying pixel-precise line/outline thickness.
    float PixelSizeInWorldUnits() const;

private:
    static constexpr int DefaultResolutionIndex = 1; // 1280x720

    // Per-sprite data uploaded to the GPU as an instanced vertex attribute buffer.
    struct SpriteInstance
    {
        glm::mat4 model;
        glm::vec4 tint;
    };

    // Per-mesh-instance data; layout mirrors SpriteInstance for simplicity.
    struct MeshInstance
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

    GLuint                        quadVao           = 0;
    GLuint                        quadVbo           = 0;
    GLuint                        instanceVbo       = 0;
    GLuint                        primitiveVao      = 0;
    GLuint                        primitiveVbo      = 0;
    GLuint                        meshInstanceVbo   = 0;
    GLuint                        defaultTextureId  = 0;
    std::shared_ptr<const Shader> spriteShader;
    std::shared_ptr<const Shader> screenShader;
    std::shared_ptr<const Shader> primitiveShader;
    std::shared_ptr<const Shader> meshShader;
    std::unique_ptr<RenderTarget> renderTarget;
    glm::mat4                     projection        = glm::mat4(1.0f);
    glm::mat4                     viewMatrix        = glm::mat4(1.0f);
    glm::mat4                     perspProjection   = glm::mat4(1.0f);

    glm::vec3 lightDirection = glm::normalize(glm::vec3(0.5f, 1.0f, 0.5f));
    glm::vec3 lightColor     = glm::vec3(1.0f);
    glm::vec3 ambientColor   = glm::vec3(0.15f);

    // Sprites queued this frame, grouped by texture ID for batched instanced drawing.
    std::unordered_map<GLuint, std::vector<SpriteInstance>> spriteQueue;

    // Composite batch key: all objects with the same geometry AND texture are
    // collapsed into one instanced draw call.
    struct MeshBatchKey
    {
        const MeshAsset* meshAsset;
        GLuint           textureId;
        bool operator==(const MeshBatchKey&) const = default;
    };

    struct MeshBatchKeyHash
    {
        size_t operator()(const MeshBatchKey& k) const
        {
            return std::hash<const MeshAsset*>()(k.meshAsset) ^
                   (std::hash<GLuint>()(k.textureId) << 1);
        }
    };

    // Meshes queued this frame, batched by (geometry, texture) pair.
    std::unordered_map<MeshBatchKey, std::vector<MeshInstance>, MeshBatchKeyHash> meshQueue;

    // MeshAssets whose VAOs have already had instance attribute pointers configured.
    // The configuration is geometry-specific and permanent (burned into the VAO).
    std::unordered_set<const MeshAsset*> meshesConfigured;

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

    // Issues one instanced draw call per unique mesh with depth testing enabled.
    // Lazily configures instance attribs on a mesh's VAO the first time it is seen.
    // Clears the queue afterwards.
    void FlushMeshes();

    // Uploads the primitive queue to the GPU and issues a single draw call.
    // Clears the queue afterwards.
    void FlushPrimitives();

    // Returns the monitor that the window most overlaps with.
    struct GLFWmonitor* GetCurrentMonitor() const;
};
