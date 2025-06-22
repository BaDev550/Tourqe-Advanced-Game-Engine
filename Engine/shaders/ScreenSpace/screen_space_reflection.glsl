#ifndef SCREEN_SPACE_REFLECTION_GLSL
#define SCREEN_SPACE_REFLECTION_GLSL

#include "screen_space_common.glsl"

vec2 ProjectToScreen(vec3 pos, mat4 proj) {
    vec4 clip = proj * vec4(pos, 1.0);
    return clip.xy / clip.w * 0.5 + 0.5;
}

vec3 ReconstructViewPos(vec2 uv, float depth, mat4 invProj) {
    vec4 clip = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    vec4 view = invProj * clip;
    return view.xyz / view.w;
}

vec3 DoSSR(
    vec3 posView,
    vec3 normView, 
    vec3 viewDir, 
    vec2 uv,
    sampler2D depthTex,
    sampler2D colorTex,
    mat4 proj, 
    mat4 invProj,
    float rough
) {
    float mask = smoothstep(0.5, 1.0, rough);
    vec3 R = reflect(-viewDir, normalize(normView));
    vec3 curr = posView;

    for (int i = 0; i < SSR_MAX_STEPS; ++i) {
        curr += R * SSR_STEP_SIZE;
        vec2 puv = ProjectToScreen(curr, proj);
        if (any(lessThan(puv, vec2(0.0))) || any(greaterThan(puv, vec2(1.0)))) break;

        float d = texture(depthTex, puv).r;
        vec3 sp = ReconstructViewPos(puv, d, invProj);
        if (abs(curr.z - sp.z) < SSR_TOLERANCE) {
            float edge = min(min(puv.x, 1.0-puv.x), min(puv.y, 1.0-puv.y));
            float fade = smoothstep(SSR_FADE_END, SSR_FADE_START, edge);
            return texture(colorTex, clampUV(puv)).rgb * fade * mask;
        }
    }
    return vec3(0.0);
}

#endif