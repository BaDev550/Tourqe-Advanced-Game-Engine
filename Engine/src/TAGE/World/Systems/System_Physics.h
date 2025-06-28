#pragma once
#include "System_Base.h"
#include "TAGE/Physics/PhysicsWorld.h"

namespace TAGE {
	class Entity;
	class System_Physics : public System {
	public:
		System_Physics(Physics::PhysicsWorld* World);
		void Update(float deltaTime) override;
		void UpdateEditor(float deltaTime) override;
	private:
		Physics::PhysicsWorld* _World;

		void RegisterRigidBody(Entity entityToRegister);
		void RegisterCollider(Entity entityToRegister);
		void DestroyPhysics(Entity entity);
		void UpdateDirty();

		friend class Scene;
		friend class SceneSerializer;
	};
}