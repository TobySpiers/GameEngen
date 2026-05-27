#version 460 core

in vec3 fragPos;
in vec3 fragNormal;
in vec2 texCoord;
in vec4 tint;

out vec4 fragColour;

uniform sampler2D meshTexture;
uniform vec3      lightDirection;
uniform vec3      lightColor;
uniform vec3      ambientColor;

void main()
{
    vec4 texColor  = texture(meshTexture, texCoord);
    if (texColor.a == 0.0) discard;
    vec3 normal    = normalize(fragNormal);
    float diff     = max(dot(normal, normalize(lightDirection)), 0.0);
    vec3  lighting = ambientColor + lightColor * diff;
    fragColour = vec4(lighting * texColor.rgb * tint.rgb, texColor.a * tint.a);
}
