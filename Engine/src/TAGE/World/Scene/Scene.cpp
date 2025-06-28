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