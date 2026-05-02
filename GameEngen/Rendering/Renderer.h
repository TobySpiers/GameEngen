#pragma once

#include "Core/ServiceLocator.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <memory>
#include <string>

class Shader;
class Texture;

class Renderer
{
public:
    static Renderer& Get() { return ServiceLocator::GetRenderer(); }
public:
    explicit Renderer(const std::string& shaderDir);
    ~Renderer();

    Renderer(const Renderer&)            = delete;
    Renderer& operator=(const Renderer&) = delete;

    // Call once per frame before any draw calls. Clears the screen and
    // updates the projection matrix if the viewport size has changed.
    void BeginFrame(int viewportWidth, int viewportHeight);

    // Draws a textured quad at the given position (top-left corner) and size,
    // with optional rotation in degrees and tint colour.
    void DrawSprite(const Texture&  texture,
                    glm::vec2       position,
                    glm::vec2       size,
                    float           rotation = 0.0f,
                    glm::vec4       tint     = glm::vec4(1.0f));

private:
    GLuint                  quadVao        = 0;
    GLuint                  quadVbo        = 0;
    std::unique_ptr<Shader> spriteShader;
    glm::mat4               projection     = glm::mat4(1.0f);
    int                     viewportWidth  = 0;
    int                     viewportHeight = 0;

    void InitQuad();
    void InitShader(const std::string& shaderDir);
};
