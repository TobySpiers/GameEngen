#include "RenderTarget.h"

#include "Log.h"

RenderTarget::RenderTarget(int width, int height)
    : width(width), height(height)
{
    Create();
}

RenderTarget::~RenderTarget()
{
    Destroy();
}

void RenderTarget::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, width, height);
}

void RenderTarget::Resize(int newWidth, int newHeight)
{
    if (newWidth == width && newHeight == height)
    {
        return;
    }

    Destroy();
    width  = newWidth;
    height = newHeight;
    Create();
}

void RenderTarget::Create()
{
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);

    glGenRenderbuffers(1, &depthRbo);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        Log::Error(LogCategory::Graphics, "RenderTarget: framebuffer incomplete");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderTarget::Destroy()
{
    if (depthRbo)  { glDeleteRenderbuffers(1, &depthRbo);  depthRbo  = 0; }
    if (textureId) { glDeleteTextures(1,      &textureId); textureId = 0; }
    if (fbo)       { glDeleteFramebuffers(1,  &fbo);       fbo       = 0; }
}
