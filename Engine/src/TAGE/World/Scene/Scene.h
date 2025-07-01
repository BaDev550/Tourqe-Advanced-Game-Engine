#pragma once
#include "TAGE/World/Systems/System_Base.h"
#include "TAGE/World/Systems/System_Renderer.h"
#include "TAGE/World/Systems/System_Physics.h"
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
		Entity FindEntityByName(std::string_view name);
		Entity GetPrimaryCamera();

		void DestroyEntity(Entity entity);
		void DuplicateEntity(Entity entity);

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

		uint GetWidth() const { return _Width; }
		uint GetHeight() const { return _Height; }

		template<typename... Components>
		auto GetEntitiesWith() {
			return _Registry.view<Components...>();
		}
	private:
		bool _Running = false;
		float _FixedTimeStep = 0.02f; 
		float _FixedTimeAccumulator = 0.0f;

		entt::registry _Registry;
		std::string _Name;
		uint _Width = 1280, _Height = 720;
		
		std::unordered_map<UUID, entt::entity> _Entities;

		MEM::Ref<System_Renderer> _RendererSystem;
		MEM::Ref<System_Physics> _PhysicsSystem;
		Physics::PhysicsWorld* _PhysicsWorld;

		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	protected:
		friend class SceneSerializer;
		friend class Entity;
	};
}