#version 460 core
#extension GL_ARB_separate_shader_objects : enable

in vec2 v_TexCoord;
in vec3 v_ViewDir;
in vec3 v_FragPos;
in vec4 v_FragPosLightSpace;
in mat3 v_TBN;

out vec4 FragColor;

#define USE_SHADOW_CALCULATION
#include "Common/light.glsl"
#include "Common/Material.glsl"
#include "Common/PBR.glsl"

uniform Material u_Material;
uniform sampler2D u_ShadowMap;

void main()
{
    vec3 viewDir = v_TBN * normalize(v_ViewDir - v_FragPos);
    vec3 fragPos = v_TBN * v_FragPos;

    vec3 normal = texture(u_Material.NormalTex, v_TexCoord).rgb;
    normal = normal * 2.0 - 1.0;
    normal = normalize(v_TBN * normal);

    vec3 albedo     = pow(texture(u_Material.DiffuseTex, v_TexCoord).rgb, vec3(2.2));
    float metallic  = texture(u_Material.MetallicTex, v_TexCoord).r;
    float roughness = texture(u_Material.RoughnessTex, v_TexCoord).r;

    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    vec3 Lo = vec3(0.0);
    for (int i = 0; i < u_LightCount; ++i)
        Lo += CalculatePBRLight(u_Lights[i], normal, viewDir, fragPos, albedo, metallic, roughness, F0);

    float shadow = 0.0;
    if (u_Lights[u_ShadowLightIndex].type == LIGHT_TYPE_DIRECTIONAL)
        shadow = ShadowCalculation(u_ShadowMap, v_FragPosLightSpace, fragPos, normal);

    vec3 ambient = vec3(0.1) * albedo * (1.0 - metallic);
    vec3 color = ambient + (1.0f - shadow) * Lo;

    FragColor = vec4(color, 1.0);
}