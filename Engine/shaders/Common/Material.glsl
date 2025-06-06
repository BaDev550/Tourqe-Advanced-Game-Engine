struct Material {
    sampler2D DiffuseTex;
    sampler2D SpecularTex;
    sampler2D NormalTex;
    sampler2D HeightTex;
    sampler2D RoughnessTex;
    sampler2D MetallicTex;
    sampler2D AmbientOcclusionTex;
};

#ifdef USE_PARALLAX_MAP
vec2 ParallaxMapping(sampler2D heightMap, vec2 texCoords, vec3 viewDir)
{ 
    float height =  texture(heightMap, texCoords).r;    
    vec2 p = viewDir.xy / viewDir.z * (height * 1.0f);
    return texCoords - p;    
} 
#endif