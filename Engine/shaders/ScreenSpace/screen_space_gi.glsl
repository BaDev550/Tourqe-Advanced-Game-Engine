#ifndef SCREEN_SPACE_GI_GLSL
#define SCREEN_SPACE_GI_GLSL

#include "screen_space_common.glsl"

uniform sampler2D u_PrevGI;
uniform mat4 u_PrevViewProj;
uniform mat4 u_CurrViewProj;

vec3 calculateSSGI(
    vec3 fragPos, 
    vec3 normal, 
    vec3 albedo, 
    vec2 uv,
    sampler2D posTex, 
    sampler2D normTex, 
    sampler2D albTex,
    sampler2D sceneTex, 
    float screenW, 
    float screenH
) {
    vec3 gi = vec3(0.0);
    int cnt = 0;
    for (int i = 0; i < NUM_SAMPLES; ++i) {
        vec2 r = vec2(
            fract(sin(dot(uv+float(i), vec2(12.9898,78.233)))*43758.5453),
            fract(sin(dot(uv+float(i)+31.7, vec2(26.651,21.821)))*12893.731)
        );
        vec3 dir = hemisphereSample(r, normal);
        vec3 sp = fragPos + dir * SAMPLE_RADIUS;
        vec2 puv = uv + dir.xy * (SAMPLE_RADIUS/vec2(screenW, screenH));
        if (any(lessThan(puv, vec2(0.0))) || any(greaterThan(puv, vec2(1.0)))) continue;

        vec3 fsp = texture(posTex, puv).rgb;
        if (fsp == vec3(0.0)) continue;
        float d = length(fsp - fragPos);
        if (d < 0.01 || d > SAMPLE_RADIUS) continue;
        float w = max(dot(normal, normalize(fsp - fragPos)), 0.0) / (1.0 + d*d);
        gi += texture(albTex, puv).rgb * w;
        cnt++;
    }
    if (cnt > 0) gi /= float(cnt);

    vec4 clip = u_CurrViewProj * vec4(fragPos,1.0);
    vec2 currUV = clip.xy/clip.w*0.5+0.5;
    vec4 prev = u_PrevViewProj * vec4(fragPos,1.0);
    vec2 prevUV = prev.xy/prev.w*0.5+0.5;
    vec3 prevGI = texture(u_PrevGI, prevUV).rgb;
    return mix(gi, prevGI, 0.5);
}

#endif