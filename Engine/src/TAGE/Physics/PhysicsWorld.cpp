#include "tagepch.h"
#include "PhysicsWorld.h"
#include "TARE/Debug/DebugRenderer.h"

namespace TAGE::Physics {

    PhysicsWorld::PhysicsWorld() {
        _Broadphase = new btDbvtBroadphase();
        _CollisionConfig = new btDefaultCollisionConfiguration();
        _Dispatcher = new btCollisionDispatcher(_CollisionConfig);
        _Solver = new btSequentialImpulseConstraintSolver();
        _DynamicsWorld = new btDiscreteDynamicsWorld(_Dispatcher, _Broadphase, _Solver, _CollisionConfig);
        _DebugRenderer = new PhysicsDebugRenderer();

        _DynamicsWorld->setGravity(btVector3(0, -9.81f, 0));
        _DynamicsWorld->setDebugDrawer(_DebugRenderer);

        TARE::Debug::DebugRenderer::Get().Init();
    }

    PhysicsWorld::~PhysicsWorld() {
        delete _DynamicsWorld;
        delete _Solver;
        delete _Dispatcher;
        delete _CollisionConfig;
        delete _Broadphase;
        delete _DebugRenderer;
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
        _DebugRenderer->BeginFrame(dt);
        _DynamicsWorld->debugDrawWorld();
        _DebugRenderer->Render(dt, viewProj);
    }

}