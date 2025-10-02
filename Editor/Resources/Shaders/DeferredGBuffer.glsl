#pragma stage:vert
#version 450 core 

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

layout(location = 0) uniform mat4 u_Model;
layout(location = 0) out vec3 FragPos;
layout(location = 1) out vec3 Normal;
layout(location = 2) out vec2 TexCoords;

#include "Common/Common.glslh"
#include "Common/Buffers.glslh"

void main() { 
    vec4 worldPos = u_Model * vec4(aPos, 1.0);
    Normal = normalize(aNormal);
    FragPos = worldPos.xyz;
    TexCoords = aTexCoords;
    gl_Position = u_Projection * u_View * worldPos;
}

#pragma stage:frag
#version 450 core

layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gAlbedo;
layout(location = 3) out vec4 gMetallicRoughness;
layout(location = 4) out int  gEntityID;

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoords;

#include "Common/PBR.glslh"

layout(location = 1) uniform float u_Metallic;
layout(location = 2) uniform float u_Roughness;
layout(location = 3) uniform bool u_HasMetallicMap;
layout(location = 4) uniform bool u_HasRoughnessMap;
layout(location = 5) uniform int u_EntityID;

void main()
{
    gPosition = vec4(FragPos, 1.0f);
    gNormal = vec4(normalize(Normal), 1.0f);
    gAlbedo = texture(u_DiffuseMap, TexCoords);
    float roughness = u_HasRoughnessMap ? texture(u_RoughnessMap, TexCoords).r : u_Roughness;
    float metallic = u_HasMetallicMap ? texture(u_MetallicMap, TexCoords).r : u_Metallic;
    gMetallicRoughness = vec4(roughness, 0.0f, 0.0f, metallic);
    gEntityID = u_EntityID;
}