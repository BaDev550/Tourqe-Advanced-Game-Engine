#pragma once
#include <glm/glm.hpp>

typedef unsigned short	ushort;
typedef unsigned long	ulong;

typedef signed char			int8;
typedef short				int16;
typedef long long			int64;
typedef unsigned int		uint;
typedef unsigned char		uint8;
typedef unsigned short		uint16;
typedef unsigned long long	uint64;

#define ASSET_CONTENT_PATH "imported/"
#define ENGINE_VERSION_MAJOR 1
#define ENGINE_VERSION_MINOR 1

#define NEAR_CLIP 0.05f
#define FAR_CLIP 250.0f

#define USE_FALLBACK_TEXTURE 1
#define SHADOW_MAP_TEXTURE_SLOT 15

#define MAX_LIGHTS 16
#define MAX_BONES 124
#define MAX_MATERIALS 32
#define MAX_INSTANCES 1024
#define MAX_BONE_INFLUENCES 4
#define SCALE_FACTOR 0.01f
#define SHADOW_MAP_RESOLUTION 2048

#define DEBUG_RENDERER_USE_GRID true
#define DEBUG_RENDERER_GRID_SIZE 100.0f
#define DEBUG_RENDERER_GRID_STEP 1.0f

#define DEBUG_RENDERER_LIGHTS false

#if USE_FALLBACK_TEXTURE
extern bool g_FallbackTextureCreated;
extern uint32_t g_FallbackTextureID;
#define FALLBACK_TEXTURE_CHECKER_SIZE 8
#define FALLBACK_TEXTURE_SIZE 128
#endif

#define BIT(x) (1 << (x))
#define BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

inline int8 QuantizeSigned(float value) { return static_cast<int8>(std::roundf(glm::clamp(value, -1.0f, 1.0f) * 127.0f)); }
inline int16 QuantizePosition(float value) { return static_cast<int16>(value * 10000.0f); }
inline uint16 QuantizeUV(float value) { return static_cast<uint16>(glm::clamp(value, 0.0f, 1.0f) * 65535.0f); }
inline uint8 QuantizeWeight(float value) { return static_cast<uint8>(glm::clamp(value, 0.0f, 1.0f) * 255.0f); }
