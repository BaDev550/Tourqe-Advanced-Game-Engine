#pragma once
#include "TAGE/World/Systems/System_Base.h"
#include "TAGE/World/Systems/System_Renderer.h"
#include "TARE/Camera/EditorCamera.h"
#include "entt/entt.hpp"
#include <vector>

namespace TAGE {
	class Entity;
	class Scene
	{
	public:
		Scene(const std::string& name);

		Entity& CreateEntity(const std::string& name);
		Entity& CreateEntityWithUUID(const std::string& name, entt::entity& ID);
		Entity& GetEntityByID(const entt::entity& ID);
		Entity GetPrimaryCamera();
		void ClearEntities();

		void OnResize(uint width, uint height) { _Width = width; _Height = height; }
		void OnUpdateRuntime(float DeltaTime);
		void OnUpdateEditor(float DeltaTime, const MEM::Ref<TARE::EditorCamera>& camera);

		template<typename T, typename... Ts>
		std::vector<MEM::Ref<Entity>> GetEntitiesWith();
		std::unordered_map<entt::entity, MEM::Ref<Entity>> GetEntities() const { return _Entities; }

		std::string& GetName() { return _Name; }
		entt::registry& GetRegistry() { return _Registry; }

		uint GetWidth() const { return _Width; }
		uint GetHeight() const { return _Height; }
	private:
		std::unordered_map<entt::entity, MEM::Ref<Entity>> _Entities;
		entt::registry _Registry;
		std::string _Name;
		uint _Width = 1280, _Height = 720;

		MEM::Ref<System_Renderer> _RendererSystem;
	protected:
		friend class SceneSerializer;
		friend class Entity;
	};

	template<typename T, typename... Ts>
	std::vector<MEM::Ref<Entity>> Scene::GetEntitiesWith()
	{
		std::vector<MEM::Ref<Entity>> result;

		auto view = _Registry.view<T, Ts...>();
		for (auto entity : view)
		{
			auto it = _Entities.find(entity);
			if (it != _Entities.end())
			{
				result.push_back(it->second);
			}
		}

		return result;
	}
}