#ifndef LIGHT_GLSL
#define LIGHT_GLSL

#define MAX_LIGHTS 64

struct Light {
    int type;
    bool castShadow;
    float intensity;
    float range;
    vec3 position;
    vec3 direction;
    vec4 color;
};

#define LIGHT_TYPE_POINT       0
#define LIGHT_TYPE_DIRECTIONAL 1
#define LIGHT_TYPE_SPOT        2

#endif