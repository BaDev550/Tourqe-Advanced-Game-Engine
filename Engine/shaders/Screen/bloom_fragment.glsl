#version 460 core

in vec2 v_TexCoord;
out vec4 FragColor;

uniform sampler2D u_Scene;
uniform float u_Threshold = 1.0;

void main() {
    vec3 color = texture(u_Scene, v_TexCoord).rgb;
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    FragColor = brightness > u_Threshold ? vec4(color, 1.0) : vec4(0.0);
}
