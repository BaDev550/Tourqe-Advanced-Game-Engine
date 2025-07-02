#pragma once
#include "TAGE/World/Systems/System_Base.h"
#include "TAGE/World/Systems/System_Renderer.h"
#include "TAGE/World/Systems/System_Physics.h"
#include "TAGE/World/Components/BaseComponents.h"
#include "TAGE/World/Components/Components.h"
#include "TAGE/Physics/PhysicsWorld.h"
#include "TARE/Camera/EditorCamera.h"
#include "TAGE/Utilities/UUID.h"
#include "entt/entt.hpp"
#include <vector>

namespace TAGE {
	class Entity;
	class Scene
	{
	public:
		Scene(const std::string& name);
		~Scene();

		static MEM::Ref<Scene> Copy(MEM::Ref<Scene> other);

		Entity CreateEntity(const std::string& name);
		Entity CreateEntityWithUUID(const std::string& name, UUID ID);

		Entity GetEntityByUUID(UUID ID);
		Entity GetEntityByID(entt::entity entityID);
		Entity TryGetEntityWithUUID(UUID id) const;
		Entity FindEntityByName(std::string_view name);
		Entity GetPrimaryCamera();

		void DestroyEntity(Entity entity);
		void DuplicateEntity(Entity entity);
		void Clear();

		void OnRuntimeStart();
		void OnSimulateStart();
		void OnRuntimeStop();
		void OnSimulateStop();
		void OnUpdateRuntime(float DeltaTime);
		void OnUpdateEditor(float DeltaTime, const MEM::Ref<TARE::EditorCamera>& camera);
		void OnUpdateSimulate(float DeltaTime, const MEM::Ref<TARE::EditorCamera>& camera);
		void OnResize(uint width, uint height) { _Width = width; _Height = height; }

		void OnPhysicsStart();
		void OnPhysicsStop();

		std::string& GetName() { return _Name; }
		entt::registry& GetRegistry() { return _Registry; }
		Physics::PhysicsWorld* GetPhysicsWorld() { return _PhysicsWorld; }
		System_Physics& GetPhysicsSystem() { return *_PhysicsSystem; }
		void ConvertToLocalSpace(Entity entity);
		void ConvertToWorldSpace(Entity entity);
		glm::mat4 GetWorldSpaceTransformMatrix(Entity entity);
		TransformComponent GetWorldSpaceTransform(Entity entity);

		uint GetWidth() const { return _Width; }
		uint GetHeight() const { return _Height; }
		bool IsRunning() const { return _Running; }

		void ParentEntity(Entity entity, Entity parent);
		void UnparentEntity(Entity entity, bool convertToWorldSpace = true);

		template<typename... Components>
		auto GetEntitiesWith() { return _Registry.view<Components...>(); }
	private:
		template<typename TComponent>
		void CopyComponentIfExists(entt::entity dst, entt::registry& dstRegistry, entt::entity src)
		{
			if (_Registry.any_of<TComponent>(src))
			{
				auto& srcComponent = _Registry.get<TComponent>(src);
				dstRegistry.emplace_or_replace<TComponent>(dst, srcComponent);
			}
		}

		static MEM::Ref<Scene> CreateEmpty();
	private:
		bool _Running = false;
		float _FixedTimeStep = 0.02f; 
		float _FixedTimeAccumulator = 0.0f;

		entt::registry _Registry;
		std::string _Name;
		uint _Width = 1280, _Height = 720;
		
		std::unordered_map<UUID, Entity> _Entities;

		MEM::Ref<System_Renderer> _RendererSystem;
		MEM::Ref<System_Physics> _PhysicsSystem;
		Physics::PhysicsWorld* _PhysicsWorld;

		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	protected:
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
		friend class Entity;
		friend class Prefab;
	};
}

#include "TAGE/World/Objects/EntityFunctions.h"