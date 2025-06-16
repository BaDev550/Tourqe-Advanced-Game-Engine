#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
    vec4 viewPos = u_View * u_Model * vec4(aPos, 1.0f);
    FragPos = viewPos.xyz;
    Normal = mat3(transpose(inverse(u_View * u_Model))) * aNormal;
    TexCoords = aTexCoords;

    gl_Position = u_Projection * viewPos;
}
