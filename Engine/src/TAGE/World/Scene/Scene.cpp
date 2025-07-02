#include "tagepch.h"
#include "Scene.h"
#include "TAGE/World/Objects/Entity.h"
#include "TAGE/Scripting/ScriptEngine.h"
#include "TAGE/Application/Application.h"

namespace TAGE {
	template<typename... Component>
	static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& entites)
	{
		([&]() {
			auto view = src.view<Component>();
			for (auto e : view) {
				entt::entity dstEntityID = entites.at(src.get<IdentityComponent>(e).UniqeId);

				auto& component = src.get<Component>(e);
				dst.emplace_or_replace<Component>(dstEntityID, component);
			}
			}(), ...);
	}

	template<typename... Component>
	static void CopyComponent(ComponentGroup<Component...>, entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& entites)
	{
		CopyComponent<Component...>(dst, src, entites);
	}

	Scene::Scene(const std::string& name) : _Name(name) {
		TARE::Debug::DebugRenderer::Get().Init();

		_PhysicsSystem = MEM::MakeRef<System_Physics>(_PhysicsWorld);
		_PhysicsSystem->SetActiveScene(this);

		_RendererSystem = MEM::MakeRef<System_Renderer>(Application::Get()->GetRenderer());
		_RendererSystem->SetActiveScene(this);
	}

	Scene::~Scene() {
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
		entity.AddComponent<RelationshipComponent>();
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
		_FixedTimeAccumulator += DeltaTime;

		while (_FixedTimeAccumulator >= _FixedTimeStep)
		{
			auto view = _Registry.view<ScriptComponent>();
			for (auto e : view) {
				Entity entity = { e, this };
				ScriptEngine::OnFixedUpdateEntity(entity, _FixedTimeStep);
			}

			_PhysicsSystem->Update(_FixedTimeStep);

			_FixedTimeAccumulator -= _FixedTimeStep;
		}

		{
			auto view = _Registry.view<ScriptComponent>();
			for (auto e : view) {
				Entity entity = { e, this };
				ScriptEngine::OnUpdateEntity(entity, DeltaTime);
			}

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
		_PhysicsSystem->UpdateEditor(DeltaTime);
		_RendererSystem->SetEditorCamera(camera);
		_RendererSystem->UpdateEditor(DeltaTime);
	}

	void Scene::OnUpdateSimulate(float DeltaTime, const MEM::Ref<TARE::EditorCamera>& camera)
	{
		_PhysicsSystem->Update(DeltaTime);
		_RendererSystem->SetEditorCamera(camera);
		_RendererSystem->UpdateEditor(DeltaTime);
	}

	void Scene::OnPhysicsStart()
	{
		_PhysicsWorld = new Physics::PhysicsWorld();
		_PhysicsSystem->SetWorld(_PhysicsWorld);
		_PhysicsSystem->StartRuntime();
	}

	void Scene::OnPhysicsStop()
	{
		delete _PhysicsWorld;
		_PhysicsWorld = nullptr;
	}

	void Scene::ConvertToLocalSpace(Entity entity)
	{
		Entity parent = TryGetEntityWithUUID(entity.GetParentUUID());

		if (!parent)
			return;

		auto& transform = entity.Transform();
		glm::mat4 parentTransform = GetWorldSpaceTransformMatrix(parent);
		glm::mat4 localTransform = glm::inverse(parentTransform) * transform.GetTransform();
		transform.SetTransform(localTransform);
	}

	void Scene::ConvertToWorldSpace(Entity entity)
	{
		Entity parent = TryGetEntityWithUUID(entity.GetParentUUID());

		if (!parent)
			return;

		glm::mat4 transform = GetWorldSpaceTransformMatrix(entity);
		auto& entityTransform = entity.Transform();
		entityTransform.SetTransform(transform);
	}

	glm::mat4 Scene::GetWorldSpaceTransformMatrix(Entity entity)
	{
		glm::mat4 transform(1.0f);

		Entity parent = TryGetEntityWithUUID(entity.GetParentUUID());
		if (parent)
			transform = GetWorldSpaceTransformMatrix(parent);

		return transform * entity.Transform().GetTransform();
	}

	TransformComponent Scene::GetWorldSpaceTransform(Entity entity)
	{
		glm::mat4 transform = GetWorldSpaceTransformMatrix(entity);
		TransformComponent transformComponent;
		transformComponent.SetTransform(transform);
		return transformComponent;
	}

	void Scene::ParentEntity(Entity entity, Entity parent)
	{
		if (parent.IsDescendantOf(entity))
		{
			UnparentEntity(parent);

			Entity newParent = TryGetEntityWithUUID(entity.GetParentUUID());
			if (newParent)
			{
				UnparentEntity(entity);
				ParentEntity(parent, newParent);
			}
		}
		else
		{
			Entity previousParent = TryGetEntityWithUUID(entity.GetParentUUID());

			if (previousParent)
				UnparentEntity(entity);
		}

		entity.SetParentUUID(parent.GetUUID());
		parent.Children().push_back(entity.GetUUID());

		ConvertToLocalSpace(entity);
	}

	void Scene::UnparentEntity(Entity entity, bool convertToWorldSpace)
	{
		Entity parent = TryGetEntityWithUUID(entity.GetParentUUID());
		if (!parent)
			return;

		auto& parentChildren = parent.Children();
		parentChildren.erase(std::remove(parentChildren.begin(), parentChildren.end(), entity.GetUUID()), parentChildren.end());

		if (convertToWorldSpace)
			ConvertToWorldSpace(entity);

		entity.SetParentUUID(0);
	}

	MEM::Ref<Scene> Scene::CreateEmpty()
	{
		return MEM::MakeRef<Scene>("Empty");
	}

	void Scene::DestroyEntity(Entity entity)
	{
		_Entities.erase(entity.GetUUID());
		_Registry.destroy(entity);
	}

	void Scene::Clear()
	{
		auto view = _Registry.view<IdentityComponent, TransformComponent>();
		for (auto e : view) {
			Entity entityToDestroy = { e, this };
			DestroyEntity(entityToDestroy);
		}
	}

	void Scene::OnRuntimeStart()
	{
		_Running = true;

		{
			ScriptEngine::OnRuntimeStart(this);

			auto view = _Registry.view<ScriptComponent>();
			for (auto e : view) {
				Entity entity = { e, this };
				ScriptEngine::OnCreateEntity(entity);
			}
		}

		OnPhysicsStart();
	}

	void Scene::OnSimulateStart()
	{
		_Running = true;
		OnPhysicsStart();
	}

	void Scene::OnRuntimeStop()
	{
		_Running = false;
		ScriptEngine::OnRuntimeStop();

		OnPhysicsStop();
	}

	void Scene::OnSimulateStop()
	{
		_Running = false;
		OnPhysicsStop();
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

	Entity Scene::TryGetEntityWithUUID(UUID id) const
	{
		if (const auto iter = _Entities.find(id); iter != _Entities.end())
			return iter->second;
		return Entity{};
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
		newScene->_PhysicsSystem = MEM::MakeRef<System_Physics>(other->_PhysicsWorld);

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

		CopyComponent(AllComponents{}, dstSceneRegistry, srcSceneRegistry, enttMap);
		return newScene;
	}

	void Scene::DuplicateEntity(Entity entity)
	{
		std::string name = entity.GetName();
		Entity newEntity = CreateEntity(name);

		CopyComponentIfExists<TransformComponent>(newEntity, _Registry, entity);
		CopyComponentIfExists<MeshComponent>(newEntity, _Registry, entity);
		CopyComponentIfExists<LightComponent>(newEntity, _Registry, entity);
		CopyComponentIfExists<SkyboxComponent>(newEntity, _Registry, entity);
		CopyComponentIfExists<ScriptComponent>(newEntity, _Registry, entity);
		CopyComponentIfExists<CameraComponent>(newEntity, _Registry, entity);
		CopyComponentIfExists<RigidBodyComponent>(newEntity, _Registry, entity);
		CopyComponentIfExists<ColliderComponent>(newEntity, _Registry, entity);
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
	void Scene::OnComponentAdded<ScriptComponent>(Entity entity, ScriptComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<RigidBodyComponent>(Entity entity, RigidBodyComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<ColliderComponent>(Entity entity, ColliderComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<RelationshipComponent>(Entity entity, RelationshipComponent& component)
	{
	}
}