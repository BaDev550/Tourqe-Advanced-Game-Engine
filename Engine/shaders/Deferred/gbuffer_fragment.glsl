#version 460 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedo;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

#include "../Common/Material.glsl"
uniform Material u_Material;

void main()
{
    vec3 Albedo = texture(u_Material.DiffuseTex, TexCoords).rgb;

    gPosition = vec4(FragPos, 1.0f);
    gNormal = vec4(normalize(Normal), 1.0f);

    gAlbedo = vec4(Albedo, 1.0f);
}
