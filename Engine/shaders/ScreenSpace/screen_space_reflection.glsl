#ifndef SCREEN_SPACE_REFLECTION_GLSL
#define SCREEN_SPACE_REFLECTION_GLSL

#include "screen_space_common.glsl"

vec3 BinarySearch(in sampler2D pos, inout vec3 dir, inout vec3 hitCoord, inout float dDepth, in mat4 projection) {
    float depth;
    vec4 projectedCoord;

    for (int i = 0; i < SSR_BINARY_SEARCH_STEPS; i++) {
        projectedCoord = projection * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;

        depth = textureLod(pos, projectedCoord.xy, 2).z;
        dDepth = hitCoord.z - depth;

        dir *= 0.5;
        if(dDepth > 0.0)
            hitCoord += dir;
        else
            hitCoord -= dir;
    }

    projectedCoord = projection * vec4(hitCoord, 1.0);
    projectedCoord.xy /= projectedCoord.w;
    projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;

    return vec3(projectedCoord.xy, depth);
}

vec4 RayCast(in sampler2D pos, in vec3 dir, inout vec3 hitCoord, out float dDepth, in mat4 projection) {
    hitCoord += dir * SSR_STEP_SIZE * 2.0;

    for (int i = 0; i < SSR_MAX_STEPS; i++) {
        vec4 projectedCoord = projection * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;

        if (any(lessThan(projectedCoord.xy, vec2(0.0))) || any(greaterThan(projectedCoord.xy, vec2(1.0)))) {
            break;
        }

        float depth = textureLod(pos, projectedCoord.xy, 0).z;
        dDepth = hitCoord.z - depth;

        if (dDepth < 0.0 && dDepth > -SSR_THICKNESS) {
            return vec4(projectedCoord.xy, depth, 1.0);
        }

        hitCoord += dir * SSR_STEP_SIZE;
    }

    return vec4(0.0, 0.0, 0.0, 0.0);
}

vec3 hash(vec3 a) {
    a = fract(a * Scale);
    a += dot(a, a.yxz + K);
    return fract((a.xxy + a.yxx) * a.zyx);
}

vec3 DoSSR(
    sampler2D position,
    sampler2D sceneColor,
    vec2 texCoords,
    vec3 viewPosition,
    vec3 viewNormal,
    mat4 projection,
    float roughness,
    float metallic
) {
    vec3 reflected = normalize(reflect(normalize(viewPosition), normalize(viewNormal)));

    vec3 randomVec = hash(viewPosition);
    float jitterAmount = roughness * roughness;
    vec3 jitteredReflected = normalize(mix(reflected, randomVec, jitterAmount));

    vec3 hitPos = viewPosition;
    float dDepth;
    vec4 coords = RayCast(position, jitteredReflected, hitPos, dDepth, projection);

    if (coords.w < 0.5) {
        return vec3(0.0);
    }

    vec2 dCoords = abs(vec2(0.5) - coords.xy);
    float screenEdgeFactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);
    screenEdgeFactor = smoothstep(0.0, 0.5, screenEdgeFactor);

    float dist = length(hitPos - viewPosition);
    float distanceFade = clamp(1.0 - (dist / SSR_DISTANCE), 0.0, 1.0);

    float roughnessFade = pow(1.0 - roughness, 2.0);
    float mipLevel = roughness * MAX_REFLECTION_LOD;
    vec3 reflectedColor = textureLod(sceneColor, coords.xy, mipLevel).rgb;

    return reflectedColor * screenEdgeFactor * distanceFade * roughnessFade;
}

#endif