#ifndef SCREEN_SPACE_AO_GLSL
#define SCREEN_SPACE_AO_GLSL

#include "screen_space_common.glsl"

float CalculateSSAO(
    vec3 fragPos, 
	vec3 normal,
	vec2 uv,
    sampler2D u_gPos,
	sampler2D u_gNorm,
    float screenW, 
	float screenH
){
    float ao = 0.0;
    float depthCenter = texture(u_gPos, uv).z;
    float sigmaDepth = SAMPLE_RADIUS * 0.5;
    
    float angle = 6.2831853 * fract(sin(dot(uv, vec2(12.9898,78.233))) * 43758.5453);
    mat2 rot = mat2(cos(angle), -sin(angle), sin(angle), cos(angle));

    for (int i = 0; i < NUM_SAMPLES; ++i) {
        vec2 r = vec2(
            fract(sin(dot(uv + float(i), vec2(12.9898,78.233))) * 43758.5453),
            fract(sin(dot(uv + float(i) + 31.7, vec2(96.345,52.313))) * 35437.3453)
        );
        vec3 dir = hemisphereSample(r, normal);
        dir.xy = rot * dir.xy;
        vec3 samplePos = fragPos + dir * SAMPLE_RADIUS;
        vec2 sampleUV  = uv + dir.xy * (SAMPLE_RADIUS / vec2(screenW, screenH));
        if (any(lessThan(sampleUV, vec2(0.0))) || any(greaterThan(sampleUV, vec2(1.0)))) continue;

        float sampleDepth = texture(u_gPos, sampleUV).z;

        float range = smoothstep(0.0, SAMPLE_RADIUS, abs(depthCenter - sampleDepth));
        float bw    = bilateralWeight(depthCenter, sampleDepth, sigmaDepth);
        ao += (sampleDepth >= samplePos.z ? 1.0 : 0.0) * range * bw;
    }
    ao = clamp(1.0 - (ao / float(NUM_SAMPLES)), 0.0, 1.0);
    return ao;
}

#endif