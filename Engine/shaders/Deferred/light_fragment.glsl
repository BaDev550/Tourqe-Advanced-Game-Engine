#version 460 core
layout(location = 0) out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D u_gPos;
uniform sampler2D u_gNorm;
uniform sampler2D u_gAlb;
uniform sampler2D u_gDepth;
uniform sampler2D u_SceneColor;

#include "../Common/light.glsl"
#include "../Common/PBR.glsl"
#include "../ScreenSpace/screen_space_gi.glsl"
#include "../ScreenSpace/screen_space_ambient_occlusion.glsl"
#include "../ScreenSpace/screen_space_reflection.glsl"

uniform vec3 u_CameraPos;
uniform mat4 u_View, u_Projection, u_InverseProjection, u_InverseView;

void main() {
    vec3 FPos = texture(u_gPos, TexCoords).rgb;
    vec3 Vdir = normalize(u_CameraPos - FPos);
    vec3 N = normalize(texture(u_gNorm, TexCoords).xyz * 2.0 - 1.0);
    float M = texture(u_gNorm, TexCoords).w;
    float S = texture(u_gAlb, TexCoords).w;
    float R = texture(u_gAlb, TexCoords).r;
    vec3 A = texture(u_gAlb, TexCoords).rgb;
    vec3 VP = (u_View * vec4(FPos, 1.0)).xyz;
    vec3 VN = mat3(u_View) * N;

    vec3 F0 = vec3(0.4);
    F0 = mix(F0, A, M);
    vec3 Fresnel = fresnelSchlick(max(dot(normalize(VN), normalize(VP)), 0.0), F0);

    float shadow = 0.0;
    vec3 color = vec3(0.0);
    vec4 fragPosLightSpace = vec4(0.0);
    for (int i = 0; i < u_LightCount; ++i) {
        fragPosLightSpace = u_Lights[i].lightSpaceMatrix * vec4(FPos, 1.0);
        shadow = ShadowCalculation(u_ShadowMaps[i], fragPosLightSpace, N, Vdir);
        vec3 lighting = CalculatePBRLight(u_Lights[i], N, Vdir, FPos, A, M, S, A);
        color += lighting * (1.0 - shadow);
    }

    float screenW = float(textureSize(u_gPos, 0).x);
    float screenH = float(textureSize(u_gPos, 0).y);

    vec3 gi = calculateSSGI(FPos, N, A, TexCoords, u_gPos, u_gNorm, u_gAlb, u_SceneColor, screenW, screenH);
    float ao = CalculateSSAO(FPos, N, TexCoords, u_gPos, u_gNorm, screenW, screenH);
    vec3 ssr = DoSSR(u_gPos, u_gAlb, TexCoords, VP, VN, u_InverseView, u_Projection, Fresnel, S, M);

    vec3 reflection = ssr * (1.0 - R);
    vec3 ambient = mix(vec3(0.02) * A, vec3(0.15) * A, ao);
    vec3 indirect = ambient * ao + gi;
    vec3 finalColor = color + indirect;

    float exposure = 0.8;
    finalColor = vec3(1.0) - exp(-finalColor * exposure);
    finalColor = pow(finalColor, vec3(1.0/2.2));

    FragColor = vec4(finalColor, 1.0);
}
