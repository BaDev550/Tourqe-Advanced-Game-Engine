#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "TDefines.h"
#include "TEnums.h"
#include "TAGE/Utilities/Memory.h"

#define GLM_ENABLE_EXPERIMENTAL
#define TAGE_ENABLE_GLM_VERTEX_DATA
#include <glm/gtx/quaternion.hpp>

struct RaycastLine {
	glm::vec3 from;
	glm::vec3 to;
	glm::vec3 color;
	RayDrawType drawType;
	float time;
};