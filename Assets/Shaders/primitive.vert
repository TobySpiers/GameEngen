#version 460 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec4 aColor;

out vec4 color;

uniform mat4 projection;

void main()
{
    color       = aColor;
    gl_Position = projection * vec4(aPos, 0.0, 1.0);
}
