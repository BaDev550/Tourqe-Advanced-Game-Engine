#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec4 aTangent;
layout (location = 4) in vec4 aBitangent;
layout (location = 5) in ivec4 aBoneIds;
layout (location = 6) in vec4  aWeights;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

const int MAX_BONES = 124;
const int MAX_BONE_INFLUENCE = 4;

uniform mat4 u_Model;
uniform bool u_UseSkinning;
uniform mat4 u_FinalBoneMatrices[MAX_BONES];

layout(std140, binding = 0) uniform CameraUBO
{
    mat4 u_View;
    mat4 u_Projection;
    mat4 u_InverseProjection;
    mat4 u_InverseView;
    mat4 u_PrevViewProj;
    mat4 u_CurrViewProj;

    vec3 u_CameraPos;
    vec3 u_CameraDir;
    vec3 u_CameraUp;

    float u_Exposure;
};

void main()
{
    vec4 worldPos = vec4(0.0);
    vec4 skinnedPos = vec4(0.0);
    vec3 skinnedNormal = vec3(0.0);
    
    if (u_UseSkinning) {
        for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
            if (aBoneIds[i] < 0 || aBoneIds[i] >= MAX_BONES) 
                continue;

            vec4 boneTransform = u_FinalBoneMatrices[aBoneIds[i]] * aWeights[i] * vec4(aPos, 1.0);
            skinnedPos += boneTransform;
            skinnedNormal += (vec3(boneTransform) * aNormal) * aWeights[i];
        }
        worldPos = u_Model * skinnedPos;
        Normal = normalize(mat3(u_Model) * skinnedNormal);
    } else {
        worldPos = u_Model * vec4(aPos, 1.0);
        Normal = normalize(mat3(u_Model) * aNormal);
    }

    FragPos = worldPos.xyz;
    TexCoords = aTexCoords;

    gl_Position = u_Projection * u_View * worldPos;
}
