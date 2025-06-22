#include "tagepch.h"
#include "Scene.h"
#include "TAGE/World/Objects/Entity.h"
#include "TAGE/World/Components/BaseComponents.h"
#include "TAGE/World/Components/RenderComponents.h"
#include "TAGE/Application/Application.h"

namespace TAGE {
	Scene::Scene(const std::string& name) : _Name(name) {
		_RendererSystem = MEM::MakeRef<System_Renderer>(Application::Get()->GetRenderer());
		_RendererSystem->SetActiveScene(this);
	}

	Entity& Scene::CreateEntity(const std::string& name)
	{
		entt::entity entityHandle = _Registry.create();
		MEM::Ref<Entity> entity = MEM::MakeRef<Entity>(entityHandle, this);
		entity->AddComponent<IdentityComponent>(name);
		entity->AddComponent<TransformComponent>();
		_Entities[entityHandle] = entity;
		return *entity;
	}

	Entity& Scene::CreateEntityWithUUID(const std::string& name, entt::entity& ID)
	{
		MEM::Ref<Entity> entity = MEM::MakeRef<Entity>(ID, this);
		entity->AddComponent<IdentityComponent>(name);
		entity->AddComponent<TransformComponent>();
		_Entities[ID] = entity;
		return *entity;
	}

	Entity& Scene::GetEntityByID(const entt::entity& ID)
	{
		if (_Entities[ID] != nullptr)
			return *_Entities[ID];
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

		_RendererSystem->Update(DeltaTime);
	}

	void Scene::OnUpdateEditor(float DeltaTime, const MEM::Ref<TARE::EditorCamera>& camera)
	{
		_RendererSystem->SetEditorCamera(camera);
		_RendererSystem->UpdateEditor(DeltaTime);
	}

	void Scene::ClearEntities()
	{
		for (auto& [entityID, entity] : _Entities)
		{
			if (entity)
				entity->Destroy();
		}
		_Entities.clear();
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
}