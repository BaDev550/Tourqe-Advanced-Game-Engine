#pragma once

#include <btBulletDynamicsCommon.h>
#include "TAGE/World/Objects/Entity.h"
#include "TAGE/Common/TEnums.h"
#include <glm/glm.hpp>

namespace TAGE {
	struct ColliderComponent {
		MEM::Ref<btCollisionShape> CollisionShape = nullptr;
		ColliderShapeType Shape = ColliderShapeType::BOX;
		CollisionResponseType ResponseType = CollisionResponseType::BLOCK;
		glm::vec3 Size = glm::vec3(1.0f);
		glm::vec3 Offset = glm::vec3(0.0f);
		bool Dirty = false;

		ColliderComponent() = default;
		ColliderComponent(const ColliderComponent&) = default;

		bool IsTrigger() const { return ResponseType == CollisionResponseType::OVERLAP; }
	};

	struct RigidBodyComponent {
		RigidBodyComponent() = default;
		RigidBodyComponent(const RigidBodyComponent&) = default;

		MEM::Ref<btRigidBody> Body = nullptr;
		MEM::Ref<btMotionState> MotionState = nullptr;
		PhysicsBodyType BodyType;
		glm::vec3 LinearFactor = glm::vec3(1.0f);
		glm::vec3 AngularFactor = glm::vec3(1.0f);

	public:
		void LockRotation(bool x, bool y, bool z) { AngularFactor = glm::vec3(x ? 0.0 : 1.0, y ? 0.0 : 1.0, z ? 0.0 : 1.0); }
		void LockMovement(bool x, bool y, bool z) { LinearFactor = glm::vec3(x ? 0.0 : 1.0, y ? 0.0 : 1.0, z ? 0.0 : 1.0); }
		void RotationLocked(bool& x, bool& y, bool& z) {
			x = AngularFactor.x == 0.0;
			y = AngularFactor.y == 0.0;
			z = AngularFactor.z == 0.0;
		}
		void MovementLocked(bool& x, bool& y, bool& z) {
			x = LinearFactor.x == 0.0;
			y = LinearFactor.y == 0.0;
			z = LinearFactor.z == 0.0;
		}

		void AddForce(const glm::vec3& force) {
			if (!Body) return;
			Body->activate();
			Body->applyCentralForce(Physics::Utils::GlmToBt(force));
		}

		void SetVelocity(const glm::vec3& vel) {
			if (!Body) return;
			Body->activate();
			Body->setLinearVelocity(Physics::Utils::GlmToBt(vel));
		}

		glm::vec3& GetVelocity() const {
			if (!Body) return glm::vec3(0.0f);
			const btVector3& vel = Body->getLinearVelocity();
			return glm::vec3(vel.getX(), vel.getY(), vel.getZ());
		}
	};
}