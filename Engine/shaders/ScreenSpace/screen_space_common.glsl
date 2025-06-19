const int NUM_SAMPLES = 16;
const float SAMPLE_RADIUS = 0.5;

vec3 hemisphereSample(vec2 rand, vec3 normal) {
    float phi = rand.x * 2.0 * 3.141592;
    float cosTheta = rand.y;
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
    
    vec3 tangent = normalize(any(normal.xz != vec2(0.0)) ? vec3(normal.z, 0.0, -normal.x) : vec3(0.0, 0.0, 1.0));
    vec3 bitangent = normalize(cross(normal, tangent));
    
    vec3 sampleVec = sinTheta * cos(phi) * tangent +
                     sinTheta * sin(phi) * bitangent +
                     cosTheta * normal;
    return normalize(sampleVec);
}