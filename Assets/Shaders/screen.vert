#version 460 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 texCoord;

void main()
{
    texCoord    = aTexCoord;

    // Unit quad (0-1) mapped to NDC (-1 to 1)
    gl_Position = vec4(aPos * 2.0 - 1.0, 0.0, 1.0);
}
