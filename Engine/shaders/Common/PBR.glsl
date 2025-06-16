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

vec3 ComputeLightDirectionAndAttenuation(Light light, vec3 fragPos, out vec3 lightDir, out float attenuation)
{
    vec3 dir = vec3(0.0);
    float att = 1.0;

    if (light.type == LIGHT_TYPE_POINT || light.type == LIGHT_TYPE_SPOT)
    {
        dir = normalize(light.position - fragPos);
        float distance = length(light.position - fragPos);
        att = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
    }
    else if (light.type == LIGHT_TYPE_DIRECTIONAL)
    {
        dir = normalize(-light.direction);
        att = 1.0;
    }

    lightDir = dir;
    attenuation = att;
    return dir;
}

float ComputeSpotFalloff(Light light, vec3 lightDir)
{
    float theta = dot(lightDir, normalize(-light.direction));
    float inner = cos(radians(light.innerCone));
    float outer = cos(radians(light.outerCone));
    return clamp((theta - outer) / (inner - outer), 0.0, 1.0);
}

vec3 ComputeCookTorranceSpecular(vec3 normal, vec3 viewDir, vec3 lightDir, vec3 F0, float roughness)
{
    vec3 H = normalize(viewDir + lightDir);
    float NDF = DistributionGGX(normal, H, roughness);
    float G   = GeometrySmith(normal, viewDir, lightDir, roughness);
    vec3 F    = fresnelSchlick(max(dot(H, viewDir), 0.0), F0);

    float denom = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.001;
    return (NDF * G * F) / denom;
}

vec3 CalculatePBRLight(
    Light light, 
    vec3 normal, 
    vec3 viewDir, 
    vec3 fragPos, 
    vec3 albedo, 
    float metallic, 
    float roughness, 
    vec3 F0)
{
    vec3 lightDir;
    float attenuation;
    ComputeLightDirectionAndAttenuation(light, fragPos, lightDir, attenuation);

    float intensity = light.intensity;

    if (light.type == LIGHT_TYPE_SPOT)
    {
        float theta = dot(lightDir, normalize(-light.direction));
        float outer = cos(radians(light.outerCone));
        if (theta < outer)
            return vec3(0.0); // fully outside spotlight
        intensity *= ComputeSpotFalloff(light, lightDir);
    }

    vec3 radiance = light.color * intensity * attenuation;
    vec3 specular = ComputeCookTorranceSpecular(normal, viewDir, lightDir, F0, roughness);

    vec3 F = fresnelSchlick(max(dot(normalize(viewDir + lightDir), viewDir), 0.0), F0);
    vec3 kS = F;
    vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);

    float NdotL = max(dot(normal, lightDir), 0.0);

    return (kD * albedo / PI + specular) * radiance * NdotL;
}
