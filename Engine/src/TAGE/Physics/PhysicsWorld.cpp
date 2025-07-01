#include "tagepch.h"
#include "PhysicsWorld.h"
#include "TARE/Debug/DebugRenderer.h"

namespace TAGE::Physics {

    PhysicsWorld::PhysicsWorld() {
        _Broadphase =      MEM::MakeScope<btDbvtBroadphase>();
        _CollisionConfig = MEM::MakeScope<btDefaultCollisionConfiguration>();
        _Solver =          MEM::MakeScope<btSequentialImpulseConstraintSolver>();
        _Dispatcher =      MEM::MakeScope<btCollisionDispatcher>(_CollisionConfig.get());
        _DynamicsWorld =   MEM::MakeScope<btDiscreteDynamicsWorld>(_Dispatcher.get(), _Broadphase.get(), _Solver.get(), _CollisionConfig.get());
        _DebugRenderer =   MEM::MakeScope<PhysicsDebugRenderer>();

        _DynamicsWorld->setGravity(btVector3(0, -9.81f, 0));
        _DynamicsWorld->setDebugDrawer(_DebugRenderer.get());
    }

    PhysicsWorld::~PhysicsWorld() {
    }

    void PhysicsWorld::StepSimulation(float deltaTime) {
        _DynamicsWorld->stepSimulation(deltaTime);
    }

    void PhysicsWorld::AddRigidBody(btRigidBody* body)
    {
        _DynamicsWorld->addRigidBody(body);
    }

    void PhysicsWorld::RemoveRigidBody(btRigidBody* body)
    {
        _DynamicsWorld->removeRigidBody(body);
    }

    void PhysicsWorld::DrawDebug(float dt, const glm::mat4& viewProj)
    {
        if (!_DynamicsWorld) return;

        auto& renderer = TARE::Debug::DebugRenderer::Get();

        renderer.BeginFrame();
        _DynamicsWorld->debugDrawWorld();
        renderer.EndFrame(dt);
        renderer.Render(viewProj);
    }

}