#define MAX_LIGHTS 16
#include "Material.glsl"

struct Light {
    int type;
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
    float range;
    float innerCone;
    float outerCone;
};

#define LIGHT_TYPE_POINT       0
#define LIGHT_TYPE_DIRECTIONAL 1
#define LIGHT_TYPE_SPOT        2

uniform Light u_Lights[MAX_LIGHTS];
uniform int u_LightCount;
uniform int u_ShadowLightIndex;

#ifdef USE_SHADOW_CALCULATION
    float ShadowCalculation(sampler2D shadowMap, vec4 fragPosLightSpace, vec3 fragPos, vec3 normal)
    {
        vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
        projCoords = projCoords * 0.5 + 0.5;

        float closestDepth = texture(shadowMap, projCoords.xy).r;
        float currentDepth = projCoords.z;

        vec3 lightDir = normalize(u_Lights[u_ShadowLightIndex].direction);
        float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.001);

        float shadow = 0.0;
        vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
        for (int x = -1; x <= 1; ++x)
        {
            for (int y = -1; y <= 1; ++y)
            {
                float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
                shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
            }
        }
        shadow /= 9.0;

        if (projCoords.z > 1.0)
            shadow = 0.0;

        return shadow;
    }
#else
    float ShadowCalculation(sampler2D shadowMap, vec4 fragPosLightSpace, vec3 fragPos, vec3 normal) { return 0.0f }
#endif