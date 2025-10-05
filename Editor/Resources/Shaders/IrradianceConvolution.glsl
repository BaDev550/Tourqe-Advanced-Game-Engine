#pragma stage:vert
#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 0) out vec3 WorldPos;

layout(location = 0) uniform mat4 u_View;
layout(location = 1) uniform mat4 u_Projection;

void main()
{
    WorldPos = aPos;
    gl_Position = u_Projection * u_View * vec4(aPos, 1.0);
}

#pragma stage:frag
#version 450 core
layout(location = 0) out vec4 FragColor;
layout(location = 0) in vec3 WorldPos;

layout(binding = 0) uniform samplerCube u_EnvironmentMap;

#include "Common/Common.glslh"

void main()
{
    vec3 N = normalize(WorldPos);
    vec3 up    = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, N));
    up         = normalize(cross(N, right));

    vec3 irradiance = vec3(0.0);

    float sampleDelta = 0.025;
    float nrSamples = 0.0;
    for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    {
        for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
        {
            vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;

            irradiance += texture(u_EnvironmentMap, sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }

    irradiance = PI * irradiance * (1.0 / nrSamples);
    FragColor = vec4(texture(u_EnvironmentMap, N).rgb, 1.0f);
}