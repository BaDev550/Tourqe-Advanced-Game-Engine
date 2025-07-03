#ifndef PBR_GLSL
#define PBR_GLSL

const float PI = 3.14159265359;
const float EPSILON = 0.0001;

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / max(denom, EPSILON);
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void GetLightProperties(Light light, vec3 fragPos, out vec3 lightDir, out float attenuation, out float intensity)
{
    attenuation = 1.0;
    intensity = light.intensity;

    if (light.type == LIGHT_TYPE_DIRECTIONAL)
    {
        lightDir = normalize(-light.direction);
    }
    else
    {
        vec3 toLight = light.position - fragPos;
        float distance = length(toLight);

        if (distance > light.range) {
            attenuation = 0.0;
            lightDir = toLight / distance;
            return;
        }

        lightDir = toLight / distance;
        attenuation = pow(clamp(1.0 - pow(distance / light.range, 4.0), 0.0, 1.0), 2.0);
        if (light.type == LIGHT_TYPE_SPOT)
        {
            float theta      = dot(lightDir, normalize(-light.direction));
            float outerCone  = cos(radians(light.outerCone));
            float innerCone  = cos(radians(light.innerCone));
            float falloff    = clamp((theta - outerCone) / (innerCone - outerCone), 0.0, 1.0);
            attenuation     *= falloff;
        }
    }
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
    float intensity = light.intensity;
    GetLightProperties(light, fragPos, lightDir, attenuation, intensity);

    if (attenuation <= 0.0) {
        return vec3(0.0);
    }

    vec3 H = normalize(viewDir + lightDir);
    float NdotL = max(dot(normal, lightDir), 0.0);
    float NdotV = max(dot(normal, viewDir), 0.0);
    float HdotV = max(dot(H, viewDir), 0.0);

    vec3 radiance = light.color * intensity * attenuation;

    float NDF = DistributionGGX(normal, H, roughness);   
    float G   = GeometrySmith(normal, viewDir, lightDir, roughness);
    vec3  F   = fresnelSchlick(HdotV, F0);

    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * NdotV * NdotL + EPSILON;
    vec3 specular     = numerator / denominator;

    vec3 kS = F;
    vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);

    vec3 diffuse = kD * albedo / PI;
    
    return (diffuse + specular) * radiance * NdotL;
}

#endif