#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D u_gPos;
uniform sampler2D u_gNorm;
uniform sampler2D u_gAlb;
uniform sampler2D u_ShadowMap;

#include "../Common/light.glsl"
#include "../Common/PBR.glsl"
#include "../ScreenSpace/screen_space_gi.glsl"
#include "../ScreenSpace/screen_space_ambient_occlusion.glsl"

uniform float u_ScreenWidth;
uniform float u_ScreenHeight;
uniform vec3 u_CameraPos;
uniform mat4 u_LightSpaceMatrix;

void main() {
    vec3 FragPos = texture(u_gPos, TexCoords).rgb;
    vec3 Normal = normalize(texture(u_gNorm, TexCoords).xyz * 2.0 - 1.0);
    float metallic = texture(u_gNorm, TexCoords).w;
    vec3 Albedo = texture(u_gAlb, TexCoords).rgb;
    float roughness = texture(u_gAlb, TexCoords).a;

    vec3 viewDir = normalize(u_CameraPos - FragPos);
    vec3 F0 = mix(vec3(0.04), Albedo, metallic);

    vec3 lighting = vec3(0.0);
    float shadow = ShadowCalculation(u_ShadowMap, texture(u_gPos, TexCoords), Normal, viewDir);
    for (int i = 0; i < u_LightCount; ++i)
    {
        Light light = u_Lights[i];
        vec3 contribution = CalculatePBRLight(light, Normal, viewDir, FragPos, Albedo, metallic, roughness, F0);
        lighting += contribution;
    }
    lighting *= (1.0 - shadow);

    vec3 indirectLighting = calculateSSGI(
        FragPos, Normal, Albedo, TexCoords,
        u_gPos, u_gNorm, u_gAlb,
        u_ScreenWidth, u_ScreenHeight
    );
    float occlusion = CalculateSSAO(
        FragPos, Normal, TexCoords,
        u_gPos, u_gNorm,
        u_ScreenWidth, u_ScreenHeight
    );

    vec3 ambient = vec3(0.6) * Albedo * occlusion;
    vec3 color = lighting + ambient + indirectLighting;
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/1.2));

    FragColor = vec4(color, 1.0);
}
