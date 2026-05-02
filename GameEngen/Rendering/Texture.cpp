#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Texture.h"

#include <cstdio>

Texture::Texture(const std::string& path)
{
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    ApplyDefaultParameters();

    stbi_set_flip_vertically_on_load(true);

    int channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 4);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    else
    {
        fprintf(stderr, "Failed to load texture: %s\n", path.c_str());
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture(int width, int height, GLenum internalFormat)
    : width(width), height(height)
{
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    ApplyDefaultParameters();
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture()
{
    glDeleteTextures(1, &textureId);
}

void Texture::Bind(int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, textureId);
}

void Texture::ApplyDefaultParameters() const
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}
