#version 460 core

layout (location = 0) in vec3 aPos;

uniform mat4 u_ViewProj;
uniform mat4 u_Model;
uniform float u_Outline = 1.08f;

void main(){
	vec3 worldPos = vec3(u_Model * vec4(aPos, 1.0f));
	gl_Position = u_ViewProj * vec4(worldPos, 1.0f);
}