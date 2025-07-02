#pragma once

#include "TAGE/Utilities/Logger.h"
#include "TAGE/World/Components/BaseComponents.h"
#include "entt/entt.hpp"

namespace TAGE {
	class Scene;
	class Entity {
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene) : _Handle(handle), _Scene(scene) {}
		Entity(const Entity& other) = default;

		template<typename T>
		bool HasComponent() const;

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args);

		template<typename T, typename... Args>
		T& AddOrReplaceComponent(Args&&... args);

		template<typename T>
		T& GetComponent();

		template<typename T>
		const T& GetComponent() const;
		
		template<typename T>
		void RemoveComponent();
		
		UUID GetUUID() { return GetComponent<IdentityComponent>().UniqeId; }
		const std::string& GetName() { return GetComponent<IdentityComponent>().Name; }

		operator entt::entity() const { return _Handle; }
		operator bool() const { return _Handle != entt::null; }
		operator uint() const { return (uint)_Handle; }

		TransformComponent& Transform() { return GetComponent<TransformComponent>(); }
		const glm::mat4& Transform() const { return GetComponent<TransformComponent>().GetTransform(); }

		bool operator==(const Entity& other) const {
			return _Handle == other._Handle && _Scene == other._Scene;
		}

		bool operator!=(const Entity& other) const {
			return !(*this == other);
		}

		Entity GetParent() const;
		void SetParent(Entity parent);

		void SetParentUUID(UUID parent) { GetComponent<RelationshipComponent>().ParentHandle = parent; }
		UUID GetParentUUID() const { return GetComponent<RelationshipComponent>().ParentHandle; }
		std::vector<UUID>& Children() { return GetComponent<RelationshipComponent>().Children; }
		const std::vector<UUID>& Children() const { return GetComponent<RelationshipComponent>().Children; }

		bool RemoveChild(Entity child)
		{
			UUID childId = child.GetUUID();
			std::vector<UUID>& children = Children();
			auto it = std::find(children.begin(), children.end(), childId);
			if (it != children.end())
			{
				children.erase(it);
				return true;
			}

			return false;
		}

		bool IsAncestorOf(Entity entity) const;
		bool IsDescendantOf(Entity entity) const { return entity.IsAncestorOf(*this); }
	private:
		entt::entity _Handle = entt::null;
		Scene* _Scene = nullptr;

		friend class Prefab;
	};
}