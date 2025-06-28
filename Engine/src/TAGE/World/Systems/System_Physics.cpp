#include "tagepch.h"
#include "System_Physics.h"
#include "TAGE/World/Components/PhysicsComponents.h"
#include "TAGE/World/Scene/Scene.h"
#include "TAGE/World/Objects/Entity.h"

namespace TAGE {
	System_Physics::System_Physics(Physics::PhysicsWorld* World)
	{
		_World = World;
	}

	void System_Physics::Update(float deltaTime)
	{
		_World->StepSimulation(deltaTime);

		UpdateDirty();

		auto view = _Scene->GetEntitiesWith<RigidBodyComponent>();
		for (auto entity : view) {
			auto physicsEntity = _Scene->GetEntityByID(entity);
			auto& rb = physicsEntity.GetComponent<RigidBodyComponent>();
			auto& transform = physicsEntity.GetComponent<TransformComponent>();

			btTransform trans;
			rb.Body->getMotionState()->getWorldTransform(trans);
			transform.Position = Physics::Utils::BtToGlm(trans.getOrigin());
			transform.Rotation = Physics::Utils::BtToGlm(trans.getRotation());
		}
	}

	void System_Physics::UpdateEditor(float deltaTime)
	{
		_World->StepSimulation(deltaTime);

		UpdateDirty();

		auto view = _Scene->GetEntitiesWith<RigidBodyComponent>();
		for (auto entity : view) {
			auto physicsEntity = _Scene->GetEntityByID(entity);
			auto& rb = physicsEntity.GetComponent<RigidBodyComponent>();
			auto& transform = physicsEntity.GetComponent<TransformComponent>();

			if (rb.BodyType == PhysicsBodyType::STATIC || rb.BodyType == PhysicsBodyType::KINEMATIC) {
				glm::vec3 pos = transform.Position;
				glm::quat rot = transform.Rotation;

				btTransform BTtransform;
				BTtransform.setOrigin(Physics::Utils::GlmToBt(pos));
				BTtransform.setRotation(Physics::Utils::GlmToBt(rot));

				rb.Body->getMotionState()->setWorldTransform(BTtransform);
				rb.Body->setWorldTransform(BTtransform);
			}
			
			if (rb.BodyType == PhysicsBodyType::DYNAMIC) {
				btTransform trans;
				rb.Body->getMotionState()->getWorldTransform(trans);
				transform.Position = Physics::Utils::BtToGlm(trans.getOrigin());
				transform.Rotation = Physics::Utils::BtToGlm(trans.getRotation());
			}

		} // TESTING
	}

	void System_Physics::RegisterRigidBody(Entity entityToRegister)
	{
		auto& rb = entityToRegister.GetComponent<RigidBodyComponent>();
		ColliderComponent* collider = nullptr;
		bool hasCollider = false;
		if (entityToRegister.HasComponent<ColliderComponent>()) {
			collider = &entityToRegister.GetComponent<ColliderComponent>();
			hasCollider = true;
		}

		auto& transform = entityToRegister.GetComponent<TransformComponent>();

		if (rb.Body != nullptr)
			return;

		btCollisionShape* shape = nullptr;
		if (hasCollider) {
			shape = collider->CollisionShape;
			if (!shape) {
				shape = Physics::Utils::CollisionShapeToBullet(collider->Shape, collider->Size);
				collider->CollisionShape = shape;
			}
		}

		btTransform startTransform;
		startTransform.setIdentity();
		startTransform.setOrigin(Physics::Utils::GlmToBt(transform.Position));
		startTransform.setRotation(Physics::Utils::GlmToBt(transform.Rotation));

		btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);

		btScalar mass = (rb.BodyType == PhysicsBodyType::DYNAMIC) ? 1.0f : 0.0f;
		btVector3 inertia(0, 0, 0);
		if (hasCollider && mass != 0.0f)
			shape->calculateLocalInertia(mass, inertia);

		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, inertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		switch (rb.BodyType) {
		case PhysicsBodyType::STATIC:
			body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
			body->setActivationState(DISABLE_SIMULATION);
			break;
		case PhysicsBodyType::KINEMATIC:
			body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
			body->setActivationState(DISABLE_DEACTIVATION);
			break;
		case PhysicsBodyType::DYNAMIC:
			break;
		}

		if (hasCollider && collider->IsTrigger()) {
			body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
		}

		body->setUserPointer(&entityToRegister);
		rb.Body = body;

		_World->AddRigidBody(body);
	}

	void System_Physics::RegisterCollider(Entity entityToRegister)
	{
		auto& collider = entityToRegister.GetComponent<ColliderComponent>();

		if (collider.CollisionShape != nullptr)
			return;

		auto& transform = entityToRegister.GetComponent<TransformComponent>();
		btCollisionShape* shape = Physics::Utils::CollisionShapeToBullet(collider.Shape, collider.Size);
		ASSERT(shape != nullptr, "Failed to create collision shape");

		collider.CollisionShape = shape;
	}

	void System_Physics::DestroyPhysics(Entity entity)
	{
		if (entity.HasComponent<RigidBodyComponent>()) {
			auto& rb = entity.GetComponent<RigidBodyComponent>();
			if (rb.Body) {
				_World->RemoveRigidBody(rb.Body);
				delete rb.Body->getMotionState();
				delete rb.Body;
				rb.Body = nullptr;
			}
		}

		if (entity.HasComponent<ColliderComponent>()) {
			auto& collider = entity.GetComponent<ColliderComponent>();
			if (collider.CollisionShape) {
				delete collider.CollisionShape;
				collider.CollisionShape = nullptr;
			}
		}
	}

	void System_Physics::UpdateDirty()
	{
		auto view = _Scene->GetEntitiesWith<RigidBodyComponent, ColliderComponent>();
		for (auto entity : view) {
			auto physicsEntity = _Scene->GetEntityByID(entity);
			auto& rb = physicsEntity.GetComponent<RigidBodyComponent>();
			auto& cc = physicsEntity.GetComponent<ColliderComponent>();

			if (cc.Dirty ||
				(rb.Body && (
					(rb.Body->getInvMass() == 0.0f && rb.BodyType == PhysicsBodyType::DYNAMIC) ||
					(rb.Body->getInvMass() != 0.0f && rb.BodyType == PhysicsBodyType::STATIC)
					))) {
				DestroyPhysics(physicsEntity);
				RegisterCollider(physicsEntity);
				RegisterRigidBody(physicsEntity);
				cc.Dirty = false;
			}

		}
	}
}