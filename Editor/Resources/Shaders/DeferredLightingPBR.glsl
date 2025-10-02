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
    vec3 A = texture(u_gAlbedo, TexCoords).rgb;
    FragColor = vec4(A, 1.0);
}