#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "TAGE/Utilities/UUID.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "TAGE/Common/TMath.h"

namespace TAGE {
	struct IdentityComponent {
		std::string Name;
		UUID UniqeId;
		std::vector<const char*> Tags;

		IdentityComponent(const std::string& name = "UNDEFINED") : Name(name) {}
		IdentityComponent(const IdentityComponent&) = default;
		IdentityComponent(const std::string& name, UUID id) : Name(name), UniqeId(id) {}
	};

	struct RelationshipComponent
	{
		UUID ParentHandle = 0;
		std::vector<UUID> Children;

		RelationshipComponent() = default;
		RelationshipComponent(const RelationshipComponent& other) = default;
		RelationshipComponent(UUID parent)
			: ParentHandle(parent) {
		}
	};

	struct TransformComponent {
		glm::vec3 Position{ 0.0f };
		glm::vec3 Scale{ 1.0f };

		glm::mat4 GetTransform() const {
			return glm::translate(glm::mat4(1.0f), Position)
				* glm::toMat4(Rotation)
				* glm::scale(glm::mat4(1.0f), Scale);
		}

		glm::vec3 GetRotationEuler() const { return RotationEuler; }
		glm::quat GetRotation() const { return Rotation; }

		void SetTransform(const glm::mat4& transform) { 
			Math::DecomposeTransform(transform, Position, Rotation, Scale); 
			RotationEuler = glm::eulerAngles(Rotation);
		}
		void SetRotationEuler(const glm::vec3& euler)
		{
			RotationEuler = euler;
			Rotation = glm::quat(RotationEuler);
		}

		void SetRotation(const glm::quat& quat)
		{
			auto wrapToPi = [](glm::vec3 v)
				{
					return glm::mod(v + glm::pi<float>(), 2.0f * glm::pi<float>()) - glm::pi<float>();
				};

			auto originalEuler = RotationEuler;
			Rotation = quat;
			RotationEuler = glm::eulerAngles(Rotation);

			glm::vec3 alternate1 = { RotationEuler.x - glm::pi<float>(), glm::pi<float>() - RotationEuler.y, RotationEuler.z - glm::pi<float>() };
			glm::vec3 alternate2 = { RotationEuler.x + glm::pi<float>(), glm::pi<float>() - RotationEuler.y, RotationEuler.z - glm::pi<float>() };
			glm::vec3 alternate3 = { RotationEuler.x + glm::pi<float>(), glm::pi<float>() - RotationEuler.y, RotationEuler.z + glm::pi<float>() };
			glm::vec3 alternate4 = { RotationEuler.x - glm::pi<float>(), glm::pi<float>() - RotationEuler.y, RotationEuler.z + glm::pi<float>() };

			float distance0 = glm::length2(wrapToPi(RotationEuler - originalEuler));
			float distance1 = glm::length2(wrapToPi(alternate1 - originalEuler));
			float distance2 = glm::length2(wrapToPi(alternate2 - originalEuler));
			float distance3 = glm::length2(wrapToPi(alternate3 - originalEuler));
			float distance4 = glm::length2(wrapToPi(alternate4 - originalEuler));

			float best = distance0;
			if (distance1 < best)
			{
				best = distance1;
				RotationEuler = alternate1;
			}
			if (distance2 < best)
			{
				best = distance2;
				RotationEuler = alternate2;
			}
			if (distance3 < best)
			{
				best = distance3;
				RotationEuler = alternate3;
			}
			if (distance4 < best)
			{
				best = distance4;
				RotationEuler = alternate4;
			}

			RotationEuler = wrapToPi(RotationEuler);
		}

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
	private:
		glm::quat Rotation{ 1.0f, 0.0f, 0.0f, 0.0f };
		glm::vec3 RotationEuler{ 0.0f, 0.0f, 0.0f };
	};
}