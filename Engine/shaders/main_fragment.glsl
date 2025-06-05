#version 460 core
#extension GL_ARB_separate_shader_objects : enable

in vec2 v_TexCoord;
in vec3 v_ViewDir;
in vec3 v_FragPos;
in vec4 v_FragPosLightSpace;

out vec4 FragColor;

#include "Common/light.glsl"
#include "Common/Material.glsl"
#include "Common/PBR.glsl"

uniform Material u_Material;
uniform sampler2D u_ShadowMap;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 fragPos, vec3 normal)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(u_ShadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    vec3 lightDir = normalize(u_Lights[u_ShadowLightIndex].direction);
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.001);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(u_ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    if (projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

void main()
{
    vec3 normal = texture(u_Material.NormalTex, v_TexCoord).rgb;
    normal = normalize(normal * 2.0 - 1.0);

    vec3 viewDir = normalize(v_ViewDir);
    vec3 fragPos = v_FragPos;

    vec3 albedo     = pow(texture(u_Material.DiffuseTex, v_TexCoord).rgb, vec3(2.2));
    float metallic  = texture(u_Material.MetallicTex, v_TexCoord).r;
    float roughness = texture(u_Material.RoughnessTex, v_TexCoord).r;

    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    vec3 Lo = vec3(0.0);
    for (int i = 0; i < u_LightCount; ++i)
    {
        Lo += CalculatePBRLight(u_Lights[i], normal, viewDir, fragPos, albedo, metallic, roughness, F0);
    }

    float shadow = 0.0;
    if (u_Lights[u_ShadowLightIndex].type == LIGHT_TYPE_DIRECTIONAL) {
        shadow = ShadowCalculation(v_FragPosLightSpace, fragPos, normal);
    }

    vec3 ambient = vec3(0.1) * albedo * (1.0 - metallic);
    //vec3 color = ambient + (1.0f - shadow) * Lo;
    vec3 color = ambient + Lo;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);
    //FragColor = vec4(vec3(shadow), 1.0);
}