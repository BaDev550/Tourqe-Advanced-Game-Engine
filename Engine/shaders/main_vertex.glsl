#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

uniform mat4 u_Model;
uniform mat4 u_ViewProjection;

out vec2 v_TexCoord;
out vec3 v_ViewDir;
out vec3 v_FragPos;
out vec4 v_FragPosLightSpace;

uniform vec3 u_CameraPosition;
uniform mat4 u_LightSpaceMatrix;

void main()
{
    mat3 normalMatrix = transpose(inverse(mat3(u_Model)));

    vec3 T = normalize(normalMatrix * aTangent);
    vec3 B = normalize(normalMatrix * aBitangent);
    vec3 N = normalize(normalMatrix * aNormal);
    mat3 TBN = transpose(mat3(T, B, N));

    v_FragPos = vec3(u_Model * vec4(aPos, 1.0));
    v_ViewDir = normalize(u_CameraPosition - v_FragPos);
    v_TexCoord = aTexCoord;
    v_FragPosLightSpace = u_LightSpaceMatrix * vec4(v_FragPos, 1.0);

    gl_Position = u_ViewProjection * vec4(v_FragPos, 1.0);
    gl_PointSize = gl_Position.z; 
}
