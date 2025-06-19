#include "screen_space_common.glsl"

vec3 calculateSSGI(
    vec3 fragPos,
    vec3 normal,
    vec3 albedo,
    vec2 TexCoords,
    sampler2D u_gPos,
    sampler2D u_gNorm,
    sampler2D u_gAlb,
    float screenWidth,
    float screenHeight
) {
    vec3 indirect = vec3(0.0);
    int validSamples = 0;

    for (int i = 0; i < NUM_SAMPLES; ++i) {
        vec2 rand = vec2(
            fract(sin(dot(TexCoords + float(i), vec2(12.9898,78.233))) * 43758.5453),
            fract(sin(dot(TexCoords + float(i) + 31.7, vec2(26.651,21.821))) * 12893.731)
        );

        vec3 sampleDir = hemisphereSample(rand, normal);
        vec3 samplePos = fragPos + sampleDir * SAMPLE_RADIUS;

        vec2 offsetUV = TexCoords + (sampleDir.xy * SAMPLE_RADIUS) / vec2(screenWidth, screenHeight);
        if (offsetUV.x < 0.0 || offsetUV.x > 1.0 || offsetUV.y < 0.0 || offsetUV.y > 1.0)
            continue;

        vec3 fetchedPos = texture(u_gPos, offsetUV).rgb;
        if (fetchedPos == vec3(0.0)) continue;

        vec3 dir = fetchedPos - fragPos;
        float dist = length(dir);
        if (dist < 0.01 || dist > SAMPLE_RADIUS) continue;

        dir = normalize(dir);
        float NdotD = max(dot(normal, dir), 0.0);
        float attenuation = NdotD / (1.0 + dist * dist);

        vec3 sampleAlb = texture(u_gAlb, offsetUV).rgb;

        indirect += sampleAlb * attenuation;
        validSamples++;
    }

    if (validSamples > 0) {
        indirect /= float(validSamples);
        indirect *= 0.6;
        indirect = clamp(indirect, vec3(0.0), vec3(1.0));
    }else{
        indirect = albedo * 0.6;
    }

    return indirect;
}
