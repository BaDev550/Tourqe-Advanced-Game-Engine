#pragma once

#include "TAGE/World/Scene/Scene.h"
#include "TAGE/Utilities/Logger.h"
#include "TAGE/World/Components/BaseComponents.h"
#include "entt/entt.hpp"

namespace TAGE {
	class Entity {
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene) : _Handle(handle), _Scene(scene) {}
		Entity(const Entity& other) = default;

		template<typename T>
		bool HasComponent() const { return _Scene->GetRegistry().any_of<T>(_Handle); }

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args) {
			ASSERT(!HasComponent<T>(), "Entity already has this component");
			T& component = _Scene->GetRegistry().emplace<T>(_Handle, std::forward<Args>(args)...);
			_Scene->OnComponentAdded(*this, component);

			return component;
		}

		template<typename T, typename... Args>
		T& AddOrReplaceComponent(Args&&... args) {
			T& component = _Scene->GetRegistry().emplace_or_replace<T>(_Handle, std::forward<Args>(args)...);
			_Scene->OnComponentAdded(*this, component);
			return component;
		}

		template<typename T>
		T& GetComponent() { 
			ASSERT_NOMSG(HasComponent<T>());
			return _Scene->GetRegistry().get<T>(_Handle);
		}

		template<typename T>
		const T& GetComponent() const {
			ASSERT_NOMSG(HasComponent<T>());
			return _Scene->GetRegistry().get<T>(_Handle);
		}
		
		template<typename T>
		void RemoveComponent() {
			ASSERT_NOMSG(HasComponent<T>());
			_Scene->GetRegistry().remove<T>(_Handle);
		}
		
		UUID GetUUID() {
			return GetComponent<IdentityComponent>().UniqeId;
		}

		const std::string& GetName() {
			return GetComponent<IdentityComponent>().Name;
		}

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

		Entity GetParent() const {
			return _Scene->TryGetEntityWithUUID(GetParentUUID());
		}

		void SetParent(Entity parent)
		{
			Entity currentParent = GetParent();
			if (currentParent == parent)
				return;

			if (currentParent)
				currentParent.RemoveChild(*this);
			SetParentUUID(parent.GetUUID());

			if (parent)
			{
				auto& parentChildren = parent.Children();
				UUID uuid = GetUUID();
				if (std::find(parentChildren.begin(), parentChildren.end(), uuid) == parentChildren.end())
					parentChildren.emplace_back(GetUUID());
			}
		}

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

		bool IsAncestorOf(Entity entity) const {
			const auto& children = Children();

			if (children.empty())
				return false;

			for (UUID child : children)
			{
				if (child == entity.GetUUID())
					return true;
			}

			for (UUID child : children)
			{
				if (_Scene->GetEntityByUUID(child).IsAncestorOf(entity))
					return true;
			}

			return false;
		}
		bool IsDescendantOf(Entity entity) const { return entity.IsAncestorOf(*this); }
	private:
		entt::entity _Handle = entt::null;
		Scene* _Scene = nullptr;
	};
}