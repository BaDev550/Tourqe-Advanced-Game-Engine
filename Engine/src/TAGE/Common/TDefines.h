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

#define PI 3.14159265358979323846f
#define MAX_LIGHTS 16
#define MAX_BONES 124
#define MAX_MATERIALS 32
#define MAX_INSTANCES 1024
#define MAX_BONE_INFLUENCES 4
#define SCALE_FACTOR 0.01f
#define SHADOW_MAP_RESOLUTION 2048

#define CUBEMAP_FACE_NUM 6
#define CUBEMAP_FACE_POSITIVE_X 0
#define CUBEMAP_FACE_NEGATIVE_X 1
#define CUBEMAP_FACE_POSITIVE_Y 2
#define CUBEMAP_FACE_NEGATIVE_Y 3
#define CUBEMAP_FACE_POSITIVE_Z 4
#define CUBEMAP_FACE_NEGATIVE_Z 5

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
#define BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }
#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))
