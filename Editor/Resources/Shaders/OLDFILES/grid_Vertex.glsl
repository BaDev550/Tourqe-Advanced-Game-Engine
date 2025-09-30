#version 460 core

layout(std140, set = 0, binding = 0) uniform CameraUBO
{
    mat4 u_View;
    mat4 u_Projection;
    mat4 u_InverseProjection;
    mat4 u_InverseView;
    mat4 u_PrevViewProj;
    mat4 u_CurrViewProj;

    vec3 u_CameraPos;
    vec3 u_CameraDir;
    vec3 u_CameraUp;

    float u_NearPlane;
    float u_FarPlane;
};

uniform float u_GridSize = 100.0f;

const vec3 Pos[4] = vec3[](
    vec3(-1.0, 0.0, -1.0),
    vec3( 1.0, 0.0, -1.0),
    vec3( 1.0, 0.0,  1.0),
    vec3(-1.0, 0.0,  1.0)
);

const int Indices[6] = int[6](
    0, 2, 1,
    2, 0, 3
);

out float v_GridSize;
out vec3 v_WorldPos;
out vec3 v_CameraPosition;

void main()
{
    int Index = Indices[gl_VertexID];
    vec3 vPos3D = Pos[Index] * u_GridSize;

    vPos3D.x += u_CameraPos.x;
    vPos3D.z += u_CameraPos.z;
    v_GridSize = u_GridSize;
    v_CameraPosition = u_CameraPos;

    vec4 vPos = vec4(vPos3D, 1.0);
    gl_Position = u_Projection * u_View * vPos;

    v_WorldPos = vPos3D;
}
