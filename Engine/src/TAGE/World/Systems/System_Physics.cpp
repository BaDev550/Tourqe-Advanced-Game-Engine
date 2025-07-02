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

		auto view = _Scene->GetEntitiesWith<RigidBodyComponent>();
		for (auto entity : view) {
			auto physicsEntity = _Scene->GetEntityByID(entity);
			auto& rb = physicsEntity.GetComponent<RigidBodyComponent>();
			auto& transform = physicsEntity.GetComponent<TransformComponent>();

			if (rb.BodyType == PhysicsBodyType::STATIC || rb.BodyType == PhysicsBodyType::KINEMATIC) {
				glm::vec3 pos = transform.Position;
				glm::quat rot = transform.GetRotation();

				btTransform BTtransform;
				BTtransform.setOrigin(Physics::Utils::GlmToBt(pos));
				BTtransform.setRotation(Physics::Utils::GlmToBt(rot));

				rb.MotionState->setWorldTransform(BTtransform);
				rb.Body->setWorldTransform(BTtransform);
			}

			if (rb.BodyType == PhysicsBodyType::DYNAMIC) {
				btTransform trans;
				rb.MotionState->getWorldTransform(trans);
				transform.Position = Physics::Utils::BtToGlm(trans.getOrigin());
				transform.SetRotation(Physics::Utils::BtToGlm(trans.getRotation()));
			}

		}
	}

	void System_Physics::UpdateEditor(float deltaTime)
	{
		auto view = _Scene->GetEntitiesWith<RigidBodyComponent>();
		for (auto entity : view) {
			auto physicsEntity = _Scene->GetEntityByID(entity);
			auto& rb = physicsEntity.GetComponent<RigidBodyComponent>();
			auto& transform = physicsEntity.GetComponent<TransformComponent>();
			glm::vec3 pos = transform.Position;
			glm::quat rot = transform.GetRotation();

			btTransform BTtransform;
			BTtransform.setOrigin(Physics::Utils::GlmToBt(pos));
			BTtransform.setRotation(Physics::Utils::GlmToBt(rot));

			if (rb.Body && rb.MotionState) {
				rb.SetVelocity({ 0,0,0 });
				rb.MotionState->setWorldTransform(BTtransform);
				rb.Body->setWorldTransform(BTtransform);
			}
		}
	}

	void System_Physics::StartRuntime()
	{
		auto view = _Scene->GetRegistry().view<RigidBodyComponent, ColliderComponent>();
		for (auto e : view) {
			Entity entity = { e, _Scene };

			auto& rb = view.get<RigidBodyComponent>(e);
			auto& cl = view.get<ColliderComponent>(e);

			if (entity.HasComponent<ColliderComponent>()) {
				RegisterCollider(entity);
			}

			if (entity.HasComponent<RigidBodyComponent>()) {
				RegisterRigidBody(entity);
			}
		}
	}

	void System_Physics::StopRuntime()
	{

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

		MEM::Ref<btCollisionShape> shape = nullptr;
		if (hasCollider) {
			if (!shape) {
				shape = Physics::Utils::CollisionShapeToBullet(entityToRegister, collider->Shape, collider->Size);
				collider->CollisionShape = shape;
			}
		}

		btTransform startTransform;
		startTransform.setIdentity();
		startTransform.setOrigin(Physics::Utils::GlmToBt(transform.Position));
		startTransform.setRotation(Physics::Utils::GlmToBt(transform.GetRotation()));

		MEM::Ref<btDefaultMotionState> motionState = MEM::MakeRef<btDefaultMotionState>(startTransform);

		btScalar mass = (rb.BodyType == PhysicsBodyType::DYNAMIC) ? 1.0f : 0.0f;
		btVector3 inertia(0, 0, 0);
		if (hasCollider && mass != 0.0f)
			shape->calculateLocalInertia(mass, inertia);

		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState.get(), shape.get(), inertia);
		MEM::Ref<btRigidBody> body = MEM::MakeRef<btRigidBody>(rbInfo);

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

		body->setLinearFactor(Physics::Utils::GlmToBt(rb.LinearFactor));
		body->setAngularFactor(Physics::Utils::GlmToBt(rb.AngularFactor));
		body->setUserPointer(&entityToRegister);
		rb.Body = std::move(body);
		rb.MotionState = std::move(motionState);

		_World->AddRigidBody(rb.Body.get());
	}

	void System_Physics::RegisterCollider(Entity entityToRegister)
	{
		auto& collider = entityToRegister.GetComponent<ColliderComponent>();

		if (collider.CollisionShape != nullptr)
			return;

		auto& transform = entityToRegister.GetComponent<TransformComponent>();
		MEM::Ref<btCollisionShape> shape = Physics::Utils::CollisionShapeToBullet(entityToRegister, collider.Shape, collider.Size);
		ASSERT(shape != nullptr, "Failed to create collision shape");

		collider.CollisionShape = std::move(shape);
	}

	void System_Physics::DestroyPhysics(Entity entity)
	{
		if (entity.HasComponent<RigidBodyComponent>()) {
			auto& rb = entity.GetComponent<RigidBodyComponent>();
			if (rb.Body) {
				_World->RemoveRigidBody(rb.Body.get());
				rb.Body = nullptr;
			}
		}

		if (entity.HasComponent<ColliderComponent>()) {
			auto& collider = entity.GetComponent<ColliderComponent>();
			if (collider.CollisionShape) {
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