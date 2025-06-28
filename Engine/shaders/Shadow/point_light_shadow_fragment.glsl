#version 460 core
in vec3 FragPos;

uniform vec3 u_LightPos;
uniform float u_FarPlane;

void main() {
    float lightDistance = length(FragPos.xyz - u_LightPos);
    lightDistance = lightDistance / u_FarPlane;
    gl_FragDepth = lightDistance;
}
