#include "tagepch.h"
#include "Scene.h"
#include "TAGE/World/Objects/Entity.h"
#include "TAGE/World/Components/BaseComponents.h"
#include "TAGE/World/Components/RenderComponents.h"
#include "TAGE/World/Components/PhysicsComponents.h"
#include "TAGE/World/Components/ScriptingComponents.h"
#include "TAGE/Application/Application.h"

namespace TAGE {
	Scene::Scene(const std::string& name) : _Name(name) {
		_RendererSystem = MEM::MakeRef<System_Renderer>(Application::Get()->GetRenderer());
		_PhysicsSystem = MEM::MakeRef<System_Physics>(&_PhysicsWorld);
		_RendererSystem->SetActiveScene(this);
		_PhysicsSystem->SetActiveScene(this);
	}

	template<typename Component>
	static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& entites)
	{
		auto view = src.view<Component>();
		for (auto e : view) {
			UUID uuid = src.get<IdentityComponent>(e).UniqeId;
			ASSERT_NOMSG(entites.find(uuid) != entites.end());
			entt::entity dstEntityID = entites.at(uuid);

			auto& component = src.get<Component>(e);
			dst.emplace_or_replace<Component>(dstEntityID, component);
		}
	}

	template<typename Component>
	static void CopyComponentIfExists(Entity dst, Entity src)
	{
		if (src.HasComponent<Component>())
			dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntityWithUUID(name, UUID());
	}

	Entity Scene::CreateEntityWithUUID(const std::string& name, UUID ID)
	{
		Entity entity = { _Registry.create(), this };
		entity.AddComponent<IdentityComponent>(name, ID);
		entity.AddComponent<TransformComponent>();
		_Entities[ID] = entity;
		return entity;
	}

	Entity Scene::GetEntityByUUID(UUID ID)
	{
		if (_Entities.find(ID) != _Entities.end())
			return { _Entities.at(ID), this};

		return {};
	}

	void Scene::OnUpdateRuntime(float DeltaTime)
	{
		{
			_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc) {
				if (!nsc.Instance)
				{
					nsc.Instance = nsc.InstantiateScript();
					nsc.Instance->_Entity = Entity{ entity, this };
					nsc.Instance->OnCreate();
				}

				nsc.Instance->OnUpdate(DeltaTime);
			});
		}

		_PhysicsSystem->Update(DeltaTime);
		_RendererSystem->Update(DeltaTime);
	}

	void Scene::OnUpdateEditor(float DeltaTime, const MEM::Ref<TARE::EditorCamera>& camera)
	{
		_PhysicsSystem->UpdateEditor(DeltaTime);
		_RendererSystem->SetEditorCamera(camera);
		_RendererSystem->UpdateEditor(DeltaTime);
	}

	void Scene::DestroyEntity(Entity entity)
	{
		_Entities.erase(entity.GetUUID());
		_Registry.destroy(entity);
	}

	void Scene::OnRuntimeStart()
	{
		_Running = true;
	}

	void Scene::OnRuntimeStop()
	{
		_Running = false;
	}

	Entity Scene::GetPrimaryCamera() {
		auto view = _Registry.view<IdentityComponent, CameraComponent>();
		for (auto entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.IsActive)
				return Entity{ entity, this };
		}
		return {};
	}

	Entity Scene::GetEntityByID(entt::entity entityID) {
		if (_Registry.valid(entityID)) {
			UUID id = _Registry.get<IdentityComponent>(entityID).UniqeId;
			return GetEntityByUUID(id);
		}
		return {};
	}

	Entity Scene::FindEntityByName(std::string_view name)
	{
		auto view = _Registry.view<IdentityComponent>();
		for (auto entity : view) {
			const auto& ic = view.get<IdentityComponent>(entity);
			if (ic.Name == name)
				return Entity{ entity, this };
		}
	}

	MEM::Ref<Scene> Scene::Copy(MEM::Ref<Scene> other)
	{
		MEM::Ref<Scene> newScene = MEM::MakeRef<Scene>("COPIEDSCENE");
		newScene->_Width = other->_Width;
		newScene->_Height = other->_Height;

		newScene->_RendererSystem = MEM::MakeRef<System_Renderer>(Application::Get()->GetRenderer());
		newScene->_PhysicsSystem = MEM::MakeRef<System_Physics>(&other->_PhysicsWorld);

		newScene->_RendererSystem->SetActiveScene(newScene.get());
		newScene->_PhysicsSystem->SetActiveScene(newScene.get());

		std::unordered_map<UUID, entt::entity> enttMap;

		auto& srcSceneRegistry = other->_Registry;
		auto& dstSceneRegistry = newScene->_Registry;
		auto idView = srcSceneRegistry.view<IdentityComponent>();

		for (auto entity : idView) {
			UUID id = srcSceneRegistry.get<IdentityComponent>(entity).UniqeId;
			const auto& name = srcSceneRegistry.get<IdentityComponent>(entity).Name;

			Entity newEntity = newScene->CreateEntityWithUUID(name, id);
			enttMap[id] = newEntity;
		}

		CopyComponent<TransformComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<LightComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<MeshComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<SkyboxComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<CameraComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<ColliderComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<RigidBodyComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);

		return newScene;
	}

	void Scene::DuplicateEntity(Entity entity)
	{
		std::string name = entity.GetName();
		Entity newEntity = CreateEntity(name);
		CopyComponentIfExists<TransformComponent>(newEntity, entity);
		CopyComponentIfExists<LightComponent>(newEntity, entity);
		CopyComponentIfExists<MeshComponent>(newEntity, entity);
		CopyComponentIfExists<SkyboxComponent>(newEntity, entity);
		CopyComponentIfExists<CameraComponent>(newEntity, entity);
		CopyComponentIfExists<ColliderComponent>(newEntity, entity);
		CopyComponentIfExists<RigidBodyComponent>(newEntity, entity);
	}

template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		static_assert(sizeof(T) == 0);
	}

	template<>
	void Scene::OnComponentAdded<IdentityComponent>(Entity entity, IdentityComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<LightComponent>(Entity entity, LightComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<MeshComponent>(Entity entity, MeshComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<SkyboxComponent>(Entity entity, SkyboxComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<RigidBodyComponent>(Entity entity, RigidBodyComponent& component)
	{
		_PhysicsSystem->RegisterRigidBody(entity);
	}

	template<>
	void Scene::OnComponentAdded<ColliderComponent>(Entity entity, ColliderComponent& component)
	{
		_PhysicsSystem->RegisterCollider(entity);
	}
}