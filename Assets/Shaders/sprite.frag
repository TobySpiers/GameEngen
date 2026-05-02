#version 460 core

in vec2 texCoord;

out vec4 fragColour;

uniform sampler2D tex;
uniform vec4 tint;

void main()
{
    fragColour = texture(tex, texCoord) * tint;
}
