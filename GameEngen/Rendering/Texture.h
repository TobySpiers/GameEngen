#pragma once

#include <glad/glad.h>
#include <string>

class Texture
{
public:
    // Load from file via stb_image
    explicit Texture(const std::string& path);

    // Create an empty texture (e.g. for compute shader output)
    Texture(int width, int height, GLenum internalFormat = GL_RGBA8);

    ~Texture();

    Texture(const Texture&)            = delete;
    Texture& operator=(const Texture&) = delete;

    void Bind(int slot = 0) const;

    int    GetWidth()  const { return width; }
    int    GetHeight() const { return height; }
    GLuint GetId()     const { return textureId; }

private:
    GLuint textureId = 0;
    int    width     = 0;
    int    height    = 0;

    void ApplyDefaultParameters() const;
};
