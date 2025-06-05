const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 CalculatePBRLight(Light light, vec3 normal, vec3 viewDir, vec3 fragPos, vec3 albedo, float metallic, float roughness, vec3 F0)
{
    vec3 lightDir;
    float attenuation = 1.0;
    float intensity = light.intensity;

    if (light.type == 0) // Point Light
    {
        lightDir = normalize(light.position - fragPos);
        float distance = length(light.position - fragPos);
        attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
    }
    else if (light.type == 1) // Directional Light
    {
        lightDir = normalize(-light.direction);
    }
    else if (light.type == 2) // Spot Light
    {
        lightDir = normalize(light.position - fragPos);
        float distance = length(light.position - fragPos);
        attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);

        float theta = dot(lightDir, normalize(-light.direction));
        float inner = cos(radians(light.innerCone));
        float outer = cos(radians(light.outerCone));
        float falloff = clamp((theta - outer) / (inner - outer), 0.0, 1.0);
        intensity *= falloff;

        if (theta < outer)
            return vec3(0.0); // Outside the spotlight
    }

    vec3 H = normalize(viewDir + lightDir);
    float NDF = DistributionGGX(normal, H, roughness);
    float G   = GeometrySmith(normal, viewDir, lightDir, roughness);
    vec3 F    = fresnelSchlick(max(dot(H, viewDir), 0.0), F0);

    vec3 nominator = NDF * G * F;
    float denom = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.001;
    vec3 specular = nominator / denom;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    float NdotL = max(dot(normal, lightDir), 0.0);
    vec3 radiance = light.color * intensity * attenuation;

    return (kD * albedo / PI + specular) * radiance * NdotL;
}
