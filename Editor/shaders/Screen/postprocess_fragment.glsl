#version 460 core
out vec4 FragColor;
in vec2 v_TexCoord;

uniform sampler2D u_ScreenTexture;
uniform float u_Exposure;

vec3 ACESFilm(vec3 x) {
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main() {
    const float gamma = 2.2f;
    vec3 color = texture(u_ScreenTexture, v_TexCoord).rgb;
    color = ACESFilm(color);
    color = vec3(1.0f) - exp(-color * u_Exposure);
    color = pow(color, vec3(1.0 / gamma));

    FragColor = vec4(color, 1.0);
}
