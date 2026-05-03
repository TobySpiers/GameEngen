#version 460 core

in vec2 texCoord;

out vec4 fragColour;

uniform sampler2D screenTexture;

void main()
{
    fragColour = texture(screenTexture, texCoord);
}
