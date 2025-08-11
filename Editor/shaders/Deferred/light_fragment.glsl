#version 460 core
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) out vec4 FragColor;
layout(set = 0, binding = 0) uniform sampler2D u_gPos;
layout(set = 0, binding = 1) uniform sampler2D u_gNorm;
layout(set = 0, binding = 2) uniform sampler2D u_gAlb;
layout(set = 0, binding = 3) uniform sampler2D u_gMetallic;
layout(set = 0, binding = 4) uniform sampler2D u_gRoughness;
layout(set = 0, binding = 5) uniform sampler2D u_gAO;
layout(set = 0, binding = 6) uniform sampler2D u_gDepth;
layout(set = 0, binding = 7) uniform sampler2D u_SceneColor;
layout(set = 0, binding = 8) uniform sampler2D u_PrevGI; 

in vec2 TexCoords;
in vec3 WorldPos;

#include "../Common/light.glsl"

layout(std140, set = 0, binding = 0) uniform CameraUBO
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

    float u_NearPlane;
    float u_FarPlane;
};

layout(std430, set = 0, binding = 1) buffer LightsSSBO
{
    Light u_Lights[MAX_LIGHTS];
    int u_LightCount;
};

layout(std140, set = 0, binding = 2) uniform ScreenSpaceUBO
{
    int NUM_SAMPLES;
    float SAMPLE_RADIUS;
    int SSR_MAX_STEPS;
    int SSR_STEP_SIZE;
    int SSR_TOLERANCE;
    int SSR_DISTANCE;
    float SSR_FADE_START;
    float SSR_FADE_END;
    int SSR_BINARY_SEARCH_STEPS;
    float SSR_THICKNESS;
    float SSR_INTENSITY;
    float MAX_REFLECTION_LOD;
    float GI_INTENSITY;
};

layout (std140, set = 0, binding = 3) uniform LightSpaceMatricesUBO
{
    mat4 lightSpaceMatrices[16];
};

struct SceneData {
    bool useSSGI;
    bool useSSR;
};

#include "../Common/shadow.glsl"
#include "../Common/PBR.glsl"
#include "../ScreenSpace/screen_space_common.glsl"
#include "../ScreenSpace/screen_space_gi.glsl"
#include "../ScreenSpace/screen_space_ambient_occlusion.glsl"
#include "../ScreenSpace/screen_space_reflection.glsl"

uniform SceneData u_SceneData;

void main() {
    vec3 FPos = texture(u_gPos, TexCoords).rgb;
    vec4 gNorm = texture(u_gNorm, TexCoords);
    vec3  N = normalize(gNorm.rgb);
    vec3  D = texture(u_gDepth, TexCoords).rgb;
    float M = texture(u_gMetallic, TexCoords).r;
    float R = texture(u_gRoughness, TexCoords).r;
    float aoVal = texture(u_gAO, TexCoords).r;
    vec4 gAlb = texture(u_gAlb, TexCoords);
    vec3 A = gAlb.rgb;

    vec3 Vdir = normalize(u_CameraPos - FPos);
    vec3 VP = (u_View * vec4(FPos, 1.0)).xyz;
    vec3 VN = normalize(mat3(u_View) * N);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, A, M);
    vec3 F_reflect = fresnelSchlick(max(dot(N, Vdir), 0.0), F0);
    vec3 F_ambient = fresnelSchlickRoughness(max(dot(N, Vdir), 0.0), F0, R);
    vec3 kS_ambient = F_ambient;
    vec3 kD_ambient = (1.0 - kS_ambient) * (1.0 - M);

    vec3 L = vec3(0.0);
    float shadow = 0.0f;
    for (int i = 0; i < MAX_LIGHTS; i++) {
        shadow = ShadowCalculation(u_View, FPos, N, u_Lights[i].direction);
        vec3 Light = CalculatePBRLight(u_Lights[i], N, Vdir, FPos, A, M, R, F0);
        L += Light * (1.0 - shadow);
    }

    vec3  gi =  vec3(0.0f);
    vec3  ssr = vec3(0.0f);
    if (u_SceneData.useSSGI) { gi =  calculateSSGI(FPos, N, A, M, R, aoVal, TexCoords, u_gPos, u_gNorm, u_gAlb, u_gMetallic, u_gRoughness, u_gAO, u_PrevGI, u_CurrViewProj, u_PrevViewProj); }
    if (u_SceneData.useSSR) {  ssr = CalculateSSR(u_gPos, u_SceneColor, TexCoords, VP, VN, u_Projection, R, M); }

    float ao =  CalculateSSAO(FPos, N, M, R, TexCoords, u_gPos, u_gNorm, u_gMetallic, u_gRoughness, u_gAO, float(textureSize(u_gPos, 0).x), float(textureSize(u_gPos, 0).y));
    vec3 ambientFallback  = kD_ambient * A * 0.1 + kS_ambient * 0.1;
    vec3 indirectDiffuse =  gi * F_reflect * A;
    vec3 indirectSpecular = ssr * F_reflect * SSR_INTENSITY;
    vec3 indirectLighting = (indirectDiffuse + ambientFallback) * ao + indirectSpecular;
    vec3 finalColor = indirectLighting + L;

    FragColor = vec4(finalColor, 1.0f);
}
