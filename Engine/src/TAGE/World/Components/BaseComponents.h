#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "TAGE/Utilities/UUID.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace TAGE {
	struct IdentityComponent {
		std::string Name;
		UUID UniqeId;
		std::vector<const char*> Tags;

		IdentityComponent(const std::string& name = "UNDEFINED") : Name(name) {}
		IdentityComponent(const std::string& name, UUID id) : Name(name), UniqeId(id) {}
	};

	struct TransformComponent {
		glm::vec3 Position{ 0.0f };
		glm::quat Rotation{ 1.0f, 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale{ 1.0f };

		glm::mat4 GetTransform() const {
			glm::mat4 translation = glm::translate(glm::mat4(1.0f), Position);
			glm::mat4 rotation = glm::mat4_cast(Rotation);
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), Scale);
			return translation * rotation * scale;
		}
	};
}