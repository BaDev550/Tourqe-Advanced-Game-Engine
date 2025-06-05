#define MAX_LIGHTS 16
#include "Material.glsl"

struct Light {
    int type;
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
    float range;
    float innerCone;
    float outerCone;
};

#define LIGHT_TYPE_POINT       0
#define LIGHT_TYPE_DIRECTIONAL 1
#define LIGHT_TYPE_SPOT        2

uniform Light u_Lights[MAX_LIGHTS];
uniform int u_LightCount;
uniform int u_ShadowLightIndex;