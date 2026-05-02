#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"

#include "Core/AssetManager.h"

#include <glm/gtc/matrix_transform.hpp>

Renderer::Renderer()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    InitQuad();
    spriteShader = AssetManager::Get().GetShader("shaders/sprite.vert", "shaders/sprite.frag");
}

Renderer::~Renderer()
{
    glDeleteVertexArrays(1, &quadVao);
    glDeleteBuffers(1, &quadVbo);
}

void Renderer::BeginFrame(int width, int height)
{
    if (width != viewportWidth || height != viewportHeight)
    {
        viewportWidth  = width;
        viewportHeight = height;
        glViewport(0, 0, width, height);
        projection = glm::ortho(
            0.0f, static_cast<float>(width),
            0.0f, static_cast<float>(height),
            -1.0f, 1.0f);
    }

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
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
        model = glm::translate(model,  glm::vec3(0.5f * size,  0.0f));
        model = glm::rotate   (model,  glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::translate(model,  glm::vec3(-0.5f * size, 0.0f));
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

