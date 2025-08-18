#version 460 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedo;
layout (location = 3) out float gMetallic;
layout (location = 4) out float gRoughness;
layout (location = 5) out float gAO;
layout (location = 6) out int  gEntityID;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

#include "../Common/Material.glsl"
uniform Material u_Material;
uniform int u_EntityID;

void main()
{
    vec3 Albedo;
    if (u_Material.HasDiffuseTex) {
        Albedo = texture(u_Material.DiffuseTex, TexCoords).rgb;
    }
    else {
        Albedo = u_Material.DiffuseColor.rgb;
    }

    if (u_Material.HasAmbientOcclusionTex) {
        gAO = texture(u_Material.AmbientOcclusionTex, TexCoords).r;
    }
    else {
        gAO = 1.0f;
    }

    float spec = texture(u_Material.SpecularTex, TexCoords).r;

    gPosition = vec4(FragPos, 1.0f);
    gNormal = vec4(Normal, 1.0f);
    gAlbedo = vec4(Albedo, 1.0f);
    gMetallic = texture(u_Material.MetallicTex, TexCoords).r;
    gRoughness = texture(u_Material.RoughnessTex, TexCoords).r;

    gEntityID = u_EntityID;
}
