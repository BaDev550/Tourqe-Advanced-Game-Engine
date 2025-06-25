#version 460 core
layout(location = 0) out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D u_gPos;
uniform sampler2D u_gNorm;
uniform sampler2D u_gAlb;
uniform sampler2D u_gDepth;

uniform sampler2D u_SceneColor;
uniform sampler2D u_ShadowMap;

#include "../Common/light.glsl"
#include "../Common/PBR.glsl"
#include "../ScreenSpace/screen_space_gi.glsl"
#include "../ScreenSpace/screen_space_ambient_occlusion.glsl"
#include "../ScreenSpace/screen_space_reflection.glsl"

uniform vec3 u_CameraPos;
uniform mat4 u_View, u_Projection, u_InverseProjection;
uniform mat4 u_LightSpaceMatrix;

void main() {
    vec3 FPos    = texture(u_gPos, TexCoords).rgb;
    vec3 N       = normalize(texture(u_gNorm, TexCoords).xyz * 2.0 - 1.0);
    float M      = texture(u_gNorm, TexCoords).w;
    vec3 A       = texture(u_gAlb, TexCoords).rgb;
    float R      = texture(u_gAlb, TexCoords).a;
    vec3 Vdir   = normalize(u_CameraPos - FPos);
    vec4 FragPosLightSpace = u_LightSpaceMatrix * vec4(FPos, 1.0);

    float sh = ShadowCalculation(u_ShadowMap, FragPosLightSpace, N, Vdir);
    vec3 L   = vec3(0.0);
    for(int i=0; i < u_LightCount; ++i) {
        L += CalculatePBRLight(
                u_Lights[i], 
                N, 
                Vdir,
                FPos, 
                A, 
                M, 
                R, 
                A
            );
    }
    L *= (1.0 - sh);
    vec3 viewPos = (u_View * vec4(FPos, 1.0)).xyz;
    vec3 viewNorm = mat3(u_View) * N;

    float ao   = CalculateSSAO(FPos, N, TexCoords, u_gPos, u_gNorm, float(textureSize(u_gPos,0).x), float(textureSize(u_gPos,0).y));
    vec3 ssr   = DoSSR(viewPos, viewNorm, Vdir, TexCoords, u_gDepth, u_SceneColor, u_Projection, u_InverseProjection, R);
    vec3 ssgi  = calculateSSGI(FPos, N, A, TexCoords, u_gPos, u_gNorm, u_gAlb, u_SceneColor, float(textureSize(u_gPos,0).x), float(textureSize(u_gPos,0).y));

    vec3 amb = mix(vec3(0.05)*A, vec3(0.35)*A, ao);
    vec3 col = amb + ssr * (1.0 - R);
    col = col / (col + vec3(1.0));
    col = pow(col, vec3(1.0/1.2));

    FragColor = vec4(col, 1.0);
}
