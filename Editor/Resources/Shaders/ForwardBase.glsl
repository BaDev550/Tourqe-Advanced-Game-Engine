#pragma stage:vert
#version 450 core 

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;
layout(location = 0) uniform mat4 u_Model;
layout(location = 0) out vec2 TextureCoords;

#include "Common/Buffers.glslh"

void main() { 
	TextureCoords = aTexCoords;
	gl_Position = u_Projection * u_View * u_Model * vec4(aPos, 1.0);
}

#pragma stage:frag
#version 450 core 

layout(location = 0) out vec4 FragColor;
layout(location = 0) in  vec2 TextureCoords;

#include "Common/PBR.glslh"

void main() { 
	vec4 diffuse = texture(u_DiffuseMap, TextureCoords);
	FragColor = diffuse;
}
