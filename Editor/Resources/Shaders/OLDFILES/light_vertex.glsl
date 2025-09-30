#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 WorldPos;

uniform mat4 u_Model;

void main() {
    TexCoords = aTexCoords;
    WorldPos = vec3(u_Model * vec4(aPos, 1.0));
    gl_Position = vec4(aPos, 1.0);
}