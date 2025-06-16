#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D u_gPos;
uniform sampler2D u_gNorm;
uniform sampler2D u_gAlb;
uniform sampler2D u_ShadowMap;

uniform vec3 u_LightPos;
uniform vec3 u_LightColor;
uniform vec3 u_CameraPos;

#include "../Common/light.glsl"

uniform float u_ScreenWidth;
uniform float u_ScreenHeight;
uniform mat4 u_LightSpaceMatrix;

const int NUM_SAMPLES = 16;
const float SAMPLE_RADIUS = 0.1;

vec3 calculateSSGI(vec3 fragPos, vec3 normal, vec3 albedo) {
    vec3 indirect = vec3(0.0);
    
    for (int i = 0; i < NUM_SAMPLES; ++i) {
        // Random offset in screen space
        vec2 offset = vec2(
            (fract(sin(dot(TexCoords.xy ,vec2(12.9898,78.233))) * 43758.5453) - 0.5),
            (fract(sin(dot(TexCoords.yx ,vec2(96.345,52.313))) * 35437.3453) - 0.5)
        ) * SAMPLE_RADIUS;

        vec2 sampleUV = TexCoords + offset / vec2(u_ScreenWidth, u_ScreenHeight);
        vec3 samplePos = texture(u_gPos, sampleUV).rgb;
        vec3 sampleNorm = normalize(texture(u_gNorm, sampleUV).rgb);
        vec3 sampleAlb = texture(u_gAlb, sampleUV).rgb;

        vec3 dir = samplePos - fragPos;
        float dist = length(dir);
        dir = normalize(dir);

        float NdotD = max(dot(normal, dir), 0.0);
        float attenuation = NdotD / (1.0 + dist * dist); // falloff

        // Indirect bounce (Lambertian diffuse)
        indirect += sampleAlb * attenuation;
    }

    indirect /= float(NUM_SAMPLES);
    indirect *= 0.5; // tweak strength
    return indirect;
}

void main() {
    vec3 FragPos = texture(u_gPos, TexCoords).rgb;
    vec3 Normal  = normalize(texture(u_gNorm, TexCoords).rgb);
    vec3 Albedo  = texture(u_gAlb, TexCoords).rgb;

    // === Direct Light ===
    vec3 lightDir = normalize(u_LightPos - FragPos);
    vec3 viewDir = normalize(u_CameraPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    vec4 FragPosLightSpace = u_LightSpaceMatrix * vec4(FragPos, 1.0);
    float shadow = ShadowCalculation(u_ShadowMap, FragPosLightSpace, Normal, lightDir);

    float diff = max(dot(Normal, lightDir), 0.0);
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);

    vec3 diffuse = diff * Albedo * u_LightColor;
    vec3 specular = spec * vec3(0.2);

    vec3 directLighting = diffuse + specular;
    vec3 indirectLighting = calculateSSGI(FragPos, Normal, Albedo);

    vec3 color = directLighting + indirectLighting;

    FragColor = vec4(color, 1.0);
}
