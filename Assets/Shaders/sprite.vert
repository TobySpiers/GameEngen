#version 460 core

// Per-vertex
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;

// Per-instance (divisor = 1)
layout(location = 2) in vec4 aModelCol0;
layout(location = 3) in vec4 aModelCol1;
layout(location = 4) in vec4 aModelCol2;
layout(location = 5) in vec4 aModelCol3;
layout(location = 6) in vec4 aTint;

out vec2 texCoord;
out vec4 tint;

uniform mat4 projection;

void main()
{
    mat4 model  = mat4(aModelCol0, aModelCol1, aModelCol2, aModelCol3);
    texCoord    = aTexCoord;
    tint        = aTint;
    gl_Position = projection * model * vec4(aPos, 0.0, 1.0);
}
