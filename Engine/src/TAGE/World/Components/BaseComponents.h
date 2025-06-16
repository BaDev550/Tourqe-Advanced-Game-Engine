#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace TAGE {
	struct IdentityComponent {
		std::string Name;
		std::vector<const char*> Tags;

		IdentityComponent(const std::string& name = "UNDEFINED") : Name(name) {}
	};

	struct TransformComponent {
		glm::vec3 Position{ 0.0f };
		glm::quat Rotation{ 1.0f, 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale{ 1.0f };
	};
}