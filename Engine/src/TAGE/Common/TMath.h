#pragma once
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/matrix4x4.h>
#include "TAGE/Utilities/Logger.h"

namespace TAGE::Math {
	glm::vec3 Scale(const glm::vec3& v, float desiredLength);
	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::quat& rotation, glm::vec3& scale);
	glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from);
}