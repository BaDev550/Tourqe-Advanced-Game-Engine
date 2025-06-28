#pragma once

#include "PhysicsWorld.h"
#include "TAGE/World/Objects/Entity.h"
#include "TAGE/Common/TTypes.h"
#include "glm/glm.hpp"

namespace TAGE::Physics { class PhysicsDebugRenderer; }
namespace TAGE::Physics::Raycast {
	struct RaycastHit
	{
		Entity* HitEntity;
		glm::vec3 point;
		glm::vec3 normal;
		float distance;
		bool hit;
	};

	class ClosestNotMeRayResultCallback : public btCollisionWorld::ClosestRayResultCallback {
	public:
		const btCollisionObject* ignoreObject;

		ClosestNotMeRayResultCallback(const btVector3& rayFromWorld, const btVector3& rayToWorld, const btCollisionObject* ignore)
			: btCollisionWorld::ClosestRayResultCallback(rayFromWorld, rayToWorld), ignoreObject(ignore) {}

		virtual bool needsCollision(btBroadphaseProxy* proxy0) const override {
			if (proxy0->m_clientObject == ignoreObject) return false;
			return btCollisionWorld::ClosestRayResultCallback::needsCollision(proxy0);
		}
	};

	class Raycaster
	{
	public:
		static void Init(PhysicsWorld& world, PhysicsDebugRenderer& _debugRenderer);
		static RaycastHit Raycast(const glm::vec3& from, const glm::vec3& to, bool ignoreSelf = true, RayDrawType draw = RayDrawType::FOR_FRAME, float draw_time = 0.0f);
		static thread_local Entity* CurrentCaller;
	private:
		static PhysicsWorld* _World;
		static PhysicsDebugRenderer* _DebugRenderer;
	};
}