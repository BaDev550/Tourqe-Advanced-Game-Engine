#pragma stage:vert
#version 450 core 

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;
layout(location = 1) out vec2 TexCoords;

void main() { 
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}

#pragma stage:frag
#version 450 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) in vec2 TexCoords;

#include "Common/Buffers.glslh"
#include "Common/PBR.glslh"

layout(binding = 0) uniform sampler2D u_gPos;
layout(binding = 1) uniform sampler2D u_gNormal;
layout(binding = 2) uniform sampler2D u_gAlbedo;
layout(binding = 3) uniform sampler2D u_gMetallicRoughness;

void main()
{
    vec3 FPos = texture(u_gPos, TexCoords).rgb;
    vec4 MetalicRoughnes = texture(u_gMetallicRoughness, TexCoords);
    vec3 A = texture(u_gAlbedo, TexCoords).rgb;
	vec3 N = texture(u_gNormal, TexCoords).rgb;
    float M = MetalicRoughnes.a;
	float S = MetalicRoughnes.g;
    float R = MetalicRoughnes.r;

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, A, M);
    vec3 VDir = normalize(u_CameraPos - FPos);
    vec3 Reflect = reflect(-VDir, N);
    vec3 Lo = vec3(0.0f);

    for (int i = 0; i < PointLightCount; i++) {
        Lo += CalculatePointBPBR(u_PointLights[i], A, VDir, FPos, N, R, M, F0);
    }

    vec3 irradiance = texture(u_IrradianceMap, N).rgb;
    vec3 radiance = texture(u_RadianceMap, Reflect).rgb;
    vec3 ambient = irradiance * A / PI;

    vec3 color = ambient + Lo;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, 1.0f);
}