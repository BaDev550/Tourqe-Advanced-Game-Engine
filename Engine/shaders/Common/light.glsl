#ifndef LIGHT_GLSL
#define LIGHT_GLSL

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
    mat4 lightSpaceMatrix;
};

#define LIGHT_TYPE_POINT       0
#define LIGHT_TYPE_DIRECTIONAL 1
#define LIGHT_TYPE_SPOT        2

uniform Light u_Lights[MAX_LIGHTS];
uniform sampler2D u_ShadowMaps[MAX_LIGHTS];
uniform int u_LightCount;

float linstep(const float low, const float high, const float value) {
    return clamp((value - low) / (high - low), 0.0, 1.0);
}

float ShadowCalculation(sampler2D shadowMap, vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0 || projCoords.z > 1.0)
        return 0.0;

    float bias = max(0.001 * (1.0 - dot(normal, lightDir)), 0.0025);
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    float avgBlockerDepth = 0.0;
    int blockerCount = 0;
    int searchRadius = 3;
    for (int x = -searchRadius; x <= searchRadius; ++x)
    {
        for (int y = -searchRadius; y <= searchRadius; ++y)
        {
            float sampleDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            if (sampleDepth < projCoords.z - bias)
            {
                avgBlockerDepth += sampleDepth;
                blockerCount++;
            }
        }
    }

    float shadow = 0.0;

    if (blockerCount == 0)
    {
        shadow = 0.0;
    }
    else
    {
        avgBlockerDepth /= blockerCount;

        float penumbra = clamp((projCoords.z - avgBlockerDepth) * 80.0, 1.0, 5.0);
        int filterRadius = int(penumbra);

        for (int x = -filterRadius; x <= filterRadius; ++x)
        {
            for (int y = -filterRadius; y <= filterRadius; ++y)
            {
                float sampleDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
                shadow += projCoords.z - bias > sampleDepth ? 1.0 : 0.0;
            }
        }

        float filterArea = pow(2.0 * float(filterRadius) + 1.0, 2.0);
        shadow /= filterArea;
    }

    return shadow;
}
#endif