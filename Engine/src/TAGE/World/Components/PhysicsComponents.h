#pragma once

#include <btBulletDynamicsCommon.h>
#include "TAGE/World/Objects/Entity.h"
#include "TAGE/Common/TEnums.h"
#include <glm/glm.hpp>

namespace TAGE {
	struct ColliderComponent {
		ColliderShapeType Shape = ColliderShapeType::BOX;
		btCollisionShape* CollisionShape = nullptr;
		CollisionResponseType ResponseType = CollisionResponseType::BLOCK;
		glm::vec3 Size = glm::vec3(1.0f);
		bool Dirty = false;

		ColliderComponent() = default;
		ColliderComponent(const ColliderComponent&) = default;

		bool IsTrigger() const { return ResponseType == CollisionResponseType::OVERLAP; }
	};

	struct RigidBodyComponent {
		RigidBodyComponent() = default;
		RigidBodyComponent(const RigidBodyComponent&) = default;

		void AddForce(const glm::vec3& force) {
			Body->activate();
			Body->applyCentralForce(btVector3(force.x, force.y, force.z));
		}

		void SetVelocity(const glm::vec3& vel) {
			Body->activate();
			Body->setLinearVelocity(btVector3(vel.x, vel.y, vel.z));
		}

		glm::vec3& GetVelocity() const {
			return glm::vec3(Body->getLinearVelocity().getX(), Body->getLinearVelocity().getY(), Body->getLinearVelocity().getZ());
		}

		btRigidBody* Body = nullptr;
		PhysicsBodyType BodyType;
		btDefaultMotionState* MotionState = nullptr;
	};
}