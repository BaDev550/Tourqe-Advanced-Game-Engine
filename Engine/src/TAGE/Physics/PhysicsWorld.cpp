#include "tagepch.h"
#include "PhysicsWorld.h"
#include "TARE/Debug/DebugRenderer.h"
#include "TAGE/World/Objects/Entity.h"
#include "TAGE/World/Components/RenderComponents.h"

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

    namespace Utils {
        enum class MeshCollisionType
        {
            STATIC_TRIANGLE_MESH,
            CONVEX_HULL
        };

        MEM::Ref<btCollisionShape> AppendMeshCollision(Entity entity)
        {
            if (!entity.HasComponent<MeshComponent>())
                return nullptr;

            std::vector<glm::vec3> allVertices;
            std::vector<uint> allIndices;

            uint indexOffset = 0;
            auto& mc = entity.GetComponent<MeshComponent>();
            uint meshCount = mc.Handle->GetMeshCount();

            for (uint i = 0; i < meshCount; i++)
            {
                auto mesh = mc.Handle->GetMesh(i);
                const auto& vertices = mesh.GetVertices();
                const auto& indices = mesh.GetIndices();

                for (const auto& vertex : vertices) {
                    allVertices.push_back(vertex.pos);
                }

                for (uint index : indices) {
                    allIndices.push_back(index + indexOffset);
                }

                indexOffset += vertices.size();
            }

            if (allVertices.empty())
            {
                ASSERT(false, "Mesh has no vertices, cannot create collision shape.");
                return nullptr;
            }

            auto shape = MEM::MakeRef<btConvexHullShape>(
                (btScalar*)allVertices.data(),
                allVertices.size(),
                sizeof(glm::vec3)
            );

            return std::move(shape);
        }

        MEM::Ref<btCollisionShape> CollisionShapeToBullet(Entity entity, ColliderShapeType shape, const glm::vec3& size)
        {
            MEM::Ref<btCollisionShape> BoxShape = MEM::MakeRef<btBoxShape>(btVector3(size.x / 2.0f, size.y / 2.0f, size.z / 2.0f));
            switch (shape)
            {
            case ColliderShapeType::BOX:      return  std::move(BoxShape);
            case ColliderShapeType::SPHERE:  return   std::move(MEM::MakeRef<btSphereShape>(size.x / 2.0f));
            case ColliderShapeType::CAPSULE:   return std::move(MEM::MakeRef<btCapsuleShape>(size.x / 2.0f, size.y));
            case ColliderShapeType::MESH: {
                    MEM::Ref<btCollisionShape> meshShape = AppendMeshCollision(entity);
                    if (!meshShape)
                        return std::move(BoxShape);

                    return meshShape;
                }
            }

            return std::move(BoxShape);
        }
    }
}

