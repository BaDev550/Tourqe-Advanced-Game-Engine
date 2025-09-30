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

#define ENGINE_VERSION_MAJOR 1
#define ENGINE_VERSION_MINOR 2

#define NEAR_CLIP 0.05f
#define FAR_CLIP 250.0f

#define PI 3.14159265358979323846f

#define BIT(x) (1 << (x))
#define BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }
#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))
