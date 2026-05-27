#version 460 core

in vec2 texCoord;
in vec4 tint;

out vec4 fragColour;

uniform sampler2D tex;

void main()
{
    fragColour = texture(tex, texCoord) * tint;
    if (fragColour.a == 0.0) discard;
}
