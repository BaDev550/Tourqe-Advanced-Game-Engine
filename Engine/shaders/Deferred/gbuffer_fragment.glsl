#version 460 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedo;
layout (location = 3) out int  gEntityID;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

#include "../Common/Material.glsl"
uniform Material u_Material;
uniform int u_EntityID;

void main()
{
    vec3 Albedo = texture(u_Material.DiffuseTex, TexCoords).rgb;
    float metallic = texture(u_Material.MetallicTex, TexCoords).r;
    float spec = texture(u_Material.SpecularTex, TexCoords).r;

    gPosition = vec4(FragPos, 1.0f);
    gNormal = vec4(normalize(Normal), metallic);
    gAlbedo = vec4(Albedo, spec);

    gEntityID = u_EntityID;
}
