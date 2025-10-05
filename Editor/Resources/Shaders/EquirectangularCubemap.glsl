#pragma stage:vert
#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 0) out vec3 LocalPos;

layout(location = 0) uniform mat4 u_View;
layout(location = 1) uniform mat4 u_Projection;

void main()
{
    LocalPos = aPos;
    gl_Position = u_Projection * u_View * vec4(aPos, 1.0);
}

#pragma stage:frag
#version 450 core

layout(location = 0) out vec4 FragColor;
layout(location = 0) in vec3 LocalPos;

layout(binding = 0) uniform sampler2D u_EquirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
    vec2 uv = SampleSphericalMap(normalize(LocalPos));
    vec3 color = texture(u_EquirectangularMap, uv).rgb;
    FragColor = vec4(color, 1.0f);
}