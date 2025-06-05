#version 460 core
out vec4 FragColor;
in vec2 v_TexCoord;

uniform sampler2D u_ScreenTexture;

void main()
{
    vec3 color = texture(u_ScreenTexture, v_TexCoord).rgb;
    FragColor = vec4(color, 1.0);
}
