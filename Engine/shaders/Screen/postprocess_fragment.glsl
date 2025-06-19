#version 460 core
out vec4 FragColor;
in vec2 v_TexCoord;

uniform sampler2D u_ScreenTexture;
uniform float exposure = 1.0f;

void main()
{
    const float gamma = 2.0f;
    vec3 color = texture(u_ScreenTexture, v_TexCoord).rgb;
    vec3 mapped = color / (color + vec3(1.0f));
    mapped = pow(mapped, vec3(1.0f / gamma));

    FragColor = vec4(color, 1.0);
}
