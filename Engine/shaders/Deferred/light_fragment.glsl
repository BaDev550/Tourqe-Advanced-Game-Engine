#version 460 core
layout(location = 0) out vec4 FragColor;

in vec2 TexCoords;
in vec3 WorldPos;

uniform sampler2D u_gPos;
uniform sampler2D u_gNorm;
uniform sampler2D u_gAlb;
uniform sampler2D u_gDepth;
uniform sampler2D u_SceneColor;
uniform sampler2D u_PrevGI;

#include "../Common/light.glsl"
#include "../Common/PBR.glsl"
#include "../ScreenSpace/screen_space_gi.glsl"
#include "../ScreenSpace/screen_space_ambient_occlusion.glsl"
#include "../ScreenSpace/screen_space_reflection.glsl"

uniform vec3 u_CameraPos;
uniform mat4 u_View, u_Projection, u_InverseProjection, u_InverseView;
uniform mat4 u_PrevViewProj, u_CurrViewProj;

void main() {
    vec3 FPos = texture(u_gPos, TexCoords).rgb;
    if (dot(FPos, FPos) < 0.001) {
        discard;
    }

    vec4 gNorm = texture(u_gNorm, TexCoords);
    vec3 Vdir = normalize(u_CameraPos - FPos);
    vec3 N = normalize(gNorm.xyz);
    float M = gNorm.w;

    vec4 gAlb = texture(u_gAlb, TexCoords);
    float R = gAlb.w;
    vec3 A = gAlb.rgb;

    vec3 VP = (u_View * vec4(FPos, 1.0)).xyz;
    vec3 VN = normalize(mat3(u_View) * N);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, A, M);
    vec3 F_reflect = fresnelSchlick(max(dot(N, Vdir), 0.0), F0);

    float shadow = 0.0;
    vec3 color = vec3(0.0);
    vec4 fragPosLightSpace = vec4(0.0);
    for (int i = 0; i < u_LightCount; ++i) {
        shadow = ShadowCalculation(u_ShadowMaps[i], u_Lights[i].lightSpaceMatrix * vec4(FPos, 1.0), N, Vdir);
        vec3 lighting = CalculatePBRLight(u_Lights[i], N, Vdir, FPos, A, M, R, F0);
        color += lighting * (1.0 - shadow);
    }
    mat4 viewProj = u_Projection * u_View;

    vec3 gi = calculateSSGI(FPos, N, A, TexCoords, u_gPos, u_gNorm, u_gAlb, u_SceneColor, viewProj, u_PrevViewProj);
    float ao = CalculateSSAO(FPos, N, TexCoords, u_gPos, u_gNorm, float(textureSize(u_gPos, 0).x), float(textureSize(u_gPos, 0).y));
    vec3 ssr = DoSSR(u_gPos, u_SceneColor, TexCoords, VP, VN, u_Projection, R, M);

    vec3 kD = A * (1.0 - M);
    vec3 indirectDiffuse = gi * kD;

    vec3 ambientFallback = 0.06 * A;
    vec3 indirectSpecular = ssr * F_reflect;
    vec3 totalIndirect = ambientFallback + indirectDiffuse + indirectSpecular;

    vec3 indirectLighting = totalIndirect * ao;
    vec3 finalColor = color + indirectLighting;

    float exposure = 0.26;
    finalColor *= exposure;
    finalColor = (finalColor * (2.51 * finalColor + 0.03)) / (finalColor * (2.43 * finalColor + 0.59) + 0.14);
    finalColor = pow(finalColor, vec3(1.0/2.2));

    FragColor = vec4(finalColor, 1.0);
}
