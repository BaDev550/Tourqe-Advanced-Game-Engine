#include "tagepch.h"
#include "Raycaster.h"
#include "PhysicsDebugRenderer.h"
#include "TAGE/World/Components/PhysicsComponents.h"

namespace TAGE::Physics::Raycast {
	PhysicsWorld* Raycaster::_World;
    PhysicsDebugRenderer* Raycaster::_DebugRenderer;
    thread_local Entity* Raycaster::CurrentCaller = nullptr;

	void Raycaster::Init(PhysicsWorld& world, PhysicsDebugRenderer& _debugRenderer)
	{
		_World = &world;
        _DebugRenderer = &_debugRenderer;
	}

	RaycastHit Raycaster::Raycast(const glm::vec3& from, const glm::vec3& to, bool ignoreSelf, RayDrawType draw, float draw_time)
	{
        btVector3 start(from.x, from.y, from.z);
        btVector3 end(to.x, to.y, to.z);

        btCollisionObject* ignore = nullptr;
        if (CurrentCaller && CurrentCaller->HasComponent<RigidBodyComponent>()) {
            ignore = CurrentCaller->GetComponent<RigidBodyComponent>().Body.get();
        }

        ClosestNotMeRayResultCallback callback(start, end, ignoreSelf ? ignore : nullptr);
        _World->GetWorld()->rayTest(start, end, callback);

        RaycastHit result{};
        result.hit = callback.hasHit();

        if (result.hit)
        {
            result.point = glm::vec3(callback.m_hitPointWorld.getX(), callback.m_hitPointWorld.getY(), callback.m_hitPointWorld.getZ());
            result.normal = glm::vec3(callback.m_hitNormalWorld.getX(), callback.m_hitNormalWorld.getY(), callback.m_hitNormalWorld.getZ());
            result.distance = (result.point - from).length();
            result.HitEntity = reinterpret_cast<Entity*>(callback.m_collisionObject->getUserPointer());
        }

        if (draw == RayDrawType::FOR_DURATION) {
            btVector3 color = result.hit ? btVector3(0.0f, 1.0f, 0.0f) : btVector3(1.0f, 0.0f, 0.0f);
            btVector3 LineEnd = result.hit ? btVector3(result.point.x, result.point.y, result.point.z) : end;

            _DebugRenderer->drawLineForSeconds(start, LineEnd, color, draw_time);
        }
        return result;
	}
}