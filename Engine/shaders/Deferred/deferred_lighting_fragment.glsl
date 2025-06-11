#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D u_gPos;
uniform sampler2D u_gNorm;
uniform sampler2D u_gAlb;

uniform vec3 u_LightPos;
uniform vec3 u_LightColor;
uniform vec3 u_CameraPos;

void main() {
    vec3 FragPos = texture(u_gPos, TexCoords).rgb;
    vec3 Normal  = normalize(texture(u_gNorm, TexCoords).rgb);
    vec3 Albedo  = texture(u_gAlb, TexCoords).rgb;

    vec3 lightDir = normalize(u_LightPos - FragPos);
    vec3 viewDir = normalize(u_CameraPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float diff = max(dot(Normal, lightDir), 0.0);
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);

    vec3 diffuse = diff * Albedo * u_LightColor;
    vec3 specular = spec * vec3(0.2);

    vec3 color = diffuse + specular;

    FragColor = vec4(color, 1.0);
}
