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
    dir *= SSR_STEP_SIZE;

    float depth;
    int steps;
    vec4 projectedCoord;

    for (int i = 0; i < SSR_MAX_STEPS; i++) {
        hitCoord += dir;

        projectedCoord = projection * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;

        depth = textureLod(pos, projectedCoord.xy, 2).z;
        if (depth > 1000.0) 
            continue;

        dDepth = hitCoord.z - depth;

        if ((dir.z - dDepth) < 1.2 && dDepth <= 0.0) {
            return vec4(BinarySearch(pos, dir, hitCoord, dDepth, projection), 1.0);
        }
        steps++;
    }

    return vec4(projectedCoord.xy, depth, 0.0);
}

vec3 hash(vec3 a) {
    a = fract(a * Scale);
    a += dot(a, a.yxz + K);
    return fract((a.xxy + a.yxx) * a.zyx);
}

vec3 DoSSR(
    sampler2D position,
    sampler2D albedo,
    vec2 texCoords,
    vec3 viewPosition,
    vec3 viewNormal, 
    mat4 inversedView,
    mat4 projection,
    vec3 Fresnel,
    float Spec,
    float Metal
) {
    vec3 reflected = normalize(reflect(normalize(viewPosition), normalize(viewNormal)));
    vec3 hitPos = viewPosition;
    float dDepth;

    vec3 wp = vec3(vec4(viewPosition, 1.0) * inversedView);
    vec3 jitt = mix(vec3(0.0), hash(wp), Spec);
    vec4 coords = RayCast(position, jitt + reflected * max(minRayStep, -viewPosition.z), hitPos, dDepth, projection);

    vec2 dCoords = smoothstep(0.2, 0.6, abs(vec2(0.5) - coords.xy));
    float screenEdgeFactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);
    float multiplier = pow(Metal, reflectionSpecularFalloffExponent) * screenEdgeFactor * -reflected.z;
    vec3 SSR = textureLod(albedo, texCoords.xy, 0).rgb * clamp(multiplier, 0.0, 0.9) * Fresnel;

    return SSR;
}

#endif