#pragma stage:vert
#version 460 core
layout (location = 0) in vec3 aPos;

layout (location = 0) uniform mat4 u_Projection;
layout (location = 1) uniform mat4 u_View;

layout (location = 0) out vec3 WorldPos;

void main()
{
    WorldPos = aPos;

	mat4 rotView = mat4(mat3(u_View));
	vec4 clipPos = u_Projection * rotView * vec4(WorldPos, 1.0);

	gl_Position = clipPos.xyww;
}

#pragma stage:frag
#version 460 core

layout (location = 0) out vec4 FragColor;
layout (location = 0) in vec3 WorldPos;

layout (binding = 0) uniform samplerCube u_EnviromentMap;

void main()
{		
    vec3 envColor = texture(u_EnviromentMap, WorldPos).rgb;
    
    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2)); 
    
    FragColor = vec4(envColor, 1.0);
}