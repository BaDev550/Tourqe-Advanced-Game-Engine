#ifndef SCREEN_SPACE_GI_GLSL
#define SCREEN_SPACE_GI_GLSL

vec3 calculateSSGI(
    vec3 fragPos,
    vec3 normal,
    vec3 albedo,
    float metallic,
    float roughness,
    float ao,
    vec2 uv,
    sampler2D posTex,
    sampler2D normTex,
    sampler2D albTex,
    sampler2D metallicTex,
    sampler2D roughnessTex,
    sampler2D aoTex,
    sampler2D sceneTex,
    mat4 viewProj,
    mat4 prevViewProj
) {
    const float EPS = 1e-5;
    vec3 gi = vec3(0.0);
    int cnt = 0;

    for (int i = 0; i < NUM_SAMPLES; ++i) {
        vec2 r = vec2(
            fract(sin(dot(uv + float(i), vec2(12.9898,78.233))) * 43758.5453),
            fract(sin(dot(uv + float(i) + 31.7, vec2(26.651,21.821))) * 12893.731)
        );
        vec3 dir = hemisphereSample(r, normal);
        vec3 sp = fragPos + dir * SAMPLE_RADIUS;

        vec4 proj_sp = viewProj * vec4(sp, 1.0);
        if (proj_sp.w <= EPS) continue;
        vec2 puv = proj_sp.xy / proj_sp.w;
        puv = puv * 0.5 + 0.5;

        if (puv.x < 0.0 || puv.x > 1.0 || puv.y < 0.0 || puv.y > 1.0) continue;

        vec3 fsp = texture(posTex, puv).rgb;
        if (dot(fsp, fsp) < 0.000001) continue;
        float d = length(fsp - fragPos);
        if (d > SAMPLE_RADIUS) continue;

        vec3 dirToSample = normalize(fsp - fragPos);
        float w = max(dot(normal, dirToSample), 0.0) / (1.0 + d * d);
        vec3 sampleAlb = texture(albTex, puv).rgb;
        if (any(isnan(sampleAlb)) || any(isinf(sampleAlb))) continue;

        gi += sampleAlb * w;
        cnt++;
    }

    if (cnt > 0) {
        gi /= float(cnt);
    } else {
        gi = vec3(0.0);
    }
    gi = max(gi, vec3(0.0));
    gi = clamp(gi, vec3(0.0), vec3(50.0)); 

    vec4 clip = viewProj * vec4(fragPos, 1.0);
    vec2 currUV = clip.xy / clip.w * 0.5 + 0.5;

    vec4 prev = prevViewProj * vec4(fragPos, 1.0);
    float prevW = prev.w;
    vec2 prevUV = (prevW > EPS) ? (prev.xy / prevW * 0.5 + 0.5) : vec2(-1.0);

    bool prevValid = (prevW > EPS) &&
                     prevUV.x >= 0.0 && prevUV.x <= 1.0 &&
                     prevUV.y >= 0.0 && prevUV.y <= 1.0;

    vec3 prevGI = vec3(0.0);
    if (prevValid) {
        prevGI = texture(sceneTex, prevUV).rgb;
        if (any(isnan(prevGI)) || any(isinf(prevGI)) || length(prevGI) < EPS) {
            prevValid = false;
            prevGI = vec3(0.0);
        }
    }
    float temporalWeight = prevValid ? 0.85 : 0.0;

    vec3 blended = mix(gi, prevGI, temporalWeight);

    blended *= albedo * GI_INTENSITY * ao;

    if (any(isnan(blended)) || any(isinf(blended))) blended = vec3(0.0);
    blended = clamp(blended, vec3(0.0), vec3(100.0));

    return blended;
}


#endif