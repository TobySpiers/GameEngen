#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

// Per-instance model matrix (4 × vec4) and tint
layout(location = 3) in vec4 aModelCol0;
layout(location = 4) in vec4 aModelCol1;
layout(location = 5) in vec4 aModelCol2;
layout(location = 6) in vec4 aModelCol3;
layout(location = 7) in vec4 aTint;

out vec3 fragPos;
out vec3 fragNormal;
out vec2 texCoord;
out vec4 tint;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    mat4 model    = mat4(aModelCol0, aModelCol1, aModelCol2, aModelCol3);
    vec4 worldPos = model * vec4(aPos, 1.0);

    fragPos    = vec3(worldPos);
    fragNormal = mat3(transpose(inverse(model))) * aNormal;
    texCoord   = aTexCoord;
    tint       = aTint;

    gl_Position = projection * view * worldPos;
}
