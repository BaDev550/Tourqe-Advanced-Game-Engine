#pragma once

#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include "TAGE/Physics/PhysicsDebugRenderer.h"

namespace TAGE::Physics {
	class PhysicsWorld
	{
	public:
        PhysicsWorld();
        ~PhysicsWorld();

        void StepSimulation(float deltaTime);
		void AddRigidBody(btRigidBody* body);
		void RemoveRigidBody(btRigidBody* body);
		void DrawDebug(float dt, const glm::mat4& viewProj);

        btDiscreteDynamicsWorld* GetWorld() const { return _DynamicsWorld.get(); }

    private:
        MEM::Scope<btBroadphaseInterface> _Broadphase;
        MEM::Scope<btDefaultCollisionConfiguration> _CollisionConfig;
        MEM::Scope<btCollisionDispatcher> _Dispatcher;
        MEM::Scope<btSequentialImpulseConstraintSolver> _Solver;
        MEM::Scope<btDiscreteDynamicsWorld> _DynamicsWorld;
		MEM::Scope<PhysicsDebugRenderer> _DebugRenderer;
	};

	namespace Utils {

		inline MEM::Ref<btCollisionShape> CollisionShapeToBullet(ColliderShapeType shape, const glm::vec3& size)
		{
			switch (shape)
			{
			case ColliderShapeType::BOX:      return  std::move(MEM::MakeRef<btBoxShape>(btVector3(size.x / 2.0f, size.y / 2.0f, size.z / 2.0f)));
			case ColliderShapeType::SPHERE:  return   std::move(MEM::MakeRef<btSphereShape>(size.x / 2.0f));
			case ColliderShapeType::CAPSULE:   return std::move(MEM::MakeRef<btCapsuleShape>(size.x / 2.0f, size.y));
			case ColliderShapeType::MESH:     ASSERT(false, "Mesh colliders not supported yet."); return nullptr;
			}

			return std::move(MEM::MakeRef<btBoxShape>(btVector3(size.x / 2.0f, size.y / 2.0f, size.z / 2.0f)));
		}

		inline btVector3 GlmToBt(const glm::vec3& v) {
			return btVector3(v.x, v.y, v.z);
		}

		inline glm::vec3 BtToGlm(const btVector3& v) {
			return glm::vec3(v.x(), v.y(), v.z());
		}

		inline btQuaternion GlmToBt(const glm::quat& q) {
			return btQuaternion(q.x, q.y, q.z, q.w);
		}

		inline glm::quat BtToGlm(const btQuaternion& q) {
			return glm::quat(q.w(), q.x(), q.y(), q.z());
		}

	}
}