#ifndef LIGHT_GLSL
#define LIGHT_GLSL

#define MAX_LIGHTS 64

struct Light {
    int type;
    int castShadow;
    float intensity;

    vec3 position;
    vec3 direction;
    vec3 color;

    float range;
    float innerCone;
    float outerCone;

    mat4 lightSpaceMatrix;
};

#define LIGHT_TYPE_POINT       0
#define LIGHT_TYPE_DIRECTIONAL 1
#define LIGHT_TYPE_SPOT        2

#endif