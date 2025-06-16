#version 460 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedo;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D u_AlbedoMap;
uniform float roughness;
uniform float metallic;

void main()
{
    vec3 Albedo = texture(u_AlbedoMap, TexCoords).rgb;
    gPosition = vec4(FragPos, 1.0);
    gNormal = vec4(normalize(Normal) * 0.5 + 0.5, metallic);
    gAlbedo = vec4(Albedo, roughness);
}
