#include "screen_space_common.glsl"

float CalculateSSAO(
	vec3 fragPos,
	vec3 normal,
	vec2 TexCoords,
	sampler2D u_gPos,
	sampler2D u_gNorm,
	float screenWidth,
	float screenHeight
){
	float occlusion = 0.0;
	for (int i = 0; i < NUM_SAMPLES; ++i) {
        vec2 rand = vec2(
            fract(sin(dot(TexCoords.xy + float(i), vec2(12.9898,78.233))) * 43758.5453),
            fract(sin(dot(TexCoords.yx + float(i), vec2(96.345,52.313))) * 35437.3453)
        );

        vec3 sampleDir = hemisphereSample(rand, normal);
        vec3 samplePos = fragPos + sampleDir * SAMPLE_RADIUS;

        vec4 offset = vec4(samplePos, 1.0);
        vec4 proj = offset;
        proj.xyz /= proj.w;
        vec2 sampleUV = (proj.xy * 0.5 + 0.5);

        if (sampleUV.x < 0.0 || sampleUV.x > 1.0 || sampleUV.y < 0.0 || sampleUV.y > 1.0)
            continue;

        vec3 fetchedPos = texture(u_gPos, sampleUV).rgb;
        float rangeCheck = smoothstep(0.0, 1.0, SAMPLE_RADIUS / abs(fragPos.z - fetchedPos.z));
        occlusion += (fetchedPos.z >= samplePos.z ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = 1.0 - (occlusion / float(NUM_SAMPLES));
    return occlusion;
}