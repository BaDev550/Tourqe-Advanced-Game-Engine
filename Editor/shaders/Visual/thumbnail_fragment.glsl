#version 450 core
layout(location = 0) out vec4 FragColor;
#include "Common/Material.glsl"

in vec2 TexCoord;
uniform Material u_Material;
void main()
{
	vec3 color = texture(u_Material.DiffuseTex, TexCoord).rgb;
	FragColor = vec4(color, 1.0f);
}
