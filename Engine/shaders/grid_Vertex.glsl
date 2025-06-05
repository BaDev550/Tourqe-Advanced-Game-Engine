#version 460 core

uniform mat4 u_View;
uniform mat4 u_Projection;
uniform vec3 u_CameraPosition;
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

    vPos3D.x += u_CameraPosition.x;
    vPos3D.z += u_CameraPosition.z;
    v_GridSize = u_GridSize;
    v_CameraPosition = u_CameraPosition;

    vec4 vPos = vec4(vPos3D, 1.0);
    gl_Position = u_Projection * u_View * vPos;

    v_WorldPos = vPos3D;
}
