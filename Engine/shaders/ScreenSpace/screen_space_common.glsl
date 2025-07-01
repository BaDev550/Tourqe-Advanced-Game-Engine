#ifndef SCREEN_SPACE_COMMON_GLSL
#define SCREEN_SPACE_COMMON_GLSL

const int NUM_SAMPLES = 16;
const float SAMPLE_RADIUS = 0.5;

#define SSR_MAX_STEPS 64
#define SSR_STEP_SIZE 0.8
#define SSR_TOLERANCE 0.05
#define SSR_DISTANCE 15.0
#define SSR_FADE_START 0.1
#define SSR_FADE_END 0.01
#define SSR_BINARY_SEARCH_STEPS 5
#define SSR_THICKNESS 0.15
#define MAX_REFLECTION_LOD 4.0

const float minRayStep = 0.1;
const float reflectionSpecularFalloffExponent = 3.0;
const vec3 Scale = vec3(0.8);
const float K = 19.19;

vec3 hemisphereSample(vec2 rand, vec3 normal) {
    float phi      = rand.x * 6.2831853;
    float cosTheta = rand.y;
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    vec3 tangent   = normalize(any(normal.xz != vec2(0.0))
                           ? vec3(normal.z, 0.0, -normal.x)
                           : vec3(0.0, 0.0, 1.0));
    vec3 bitangent = normalize(cross(normal, tangent));

    return normalize(
        tangent * (sinTheta * cos(phi)) +
        bitangent * (sinTheta * sin(phi)) +
        normal  * cosTheta
    );
}

float bilateralWeight(float dCenter, float dSample, float sigmaDepth) {
    float diff = abs(dCenter - dSample);
    return exp(-diff * diff / (2.0 * sigmaDepth * sigmaDepth));
}

vec2 clampUV(vec2 uv) {
    return clamp(uv, vec2(0.001), vec2(0.999));
}

#endif