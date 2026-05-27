#pragma once

#include <glad/glad.h>

class RenderTarget
{
public:
    RenderTarget(int width, int height);
    ~RenderTarget();

    RenderTarget(const RenderTarget&)            = delete;
    RenderTarget& operator=(const RenderTarget&) = delete;

    // Binds the FBO and sets the viewport to the render target's dimensions.
    void Bind() const;

    // Recreates the FBO and texture at the new dimensions.
    void Resize(int width, int height);

    GLuint GetTextureId() const { return textureId; }
    int    GetWidth()     const { return width; }
    int    GetHeight()    const { return height; }

private:
    GLuint fbo       = 0;
    GLuint textureId = 0;
    GLuint depthRbo  = 0;
    int    width     = 0;
    int    height    = 0;

    void Create();
    void Destroy();
};
