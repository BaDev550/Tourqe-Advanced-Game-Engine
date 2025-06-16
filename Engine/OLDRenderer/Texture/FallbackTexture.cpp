#include "tagepch.h"
#include "TAGE/Common/TDefines.h"

#define USE_FALLBACK_TEXTURE 
#ifdef USE_FALLBACK_TEXTURE
bool g_FallbackTextureCreated = false;
uint32_t g_FallbackTextureID = 0;
#endif