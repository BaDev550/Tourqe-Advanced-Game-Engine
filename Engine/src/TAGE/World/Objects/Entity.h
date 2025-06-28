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
		void RemoveComponent() {
			ASSERT_NOMSG(HasComponent<T>());
			_Scene->GetRegistry().remove<T>(_Handle);
		}
		
		UUID GetUUID() {
			return GetComponent<IdentityComponent>().UniqeId;
		}

		operator entt::entity() const { return _Handle; }
		operator bool() const { return _Handle != entt::null; }
		operator uint() const { return (uint)_Handle; }

		bool operator==(const Entity& other) const {
			return _Handle == other._Handle && _Scene == other._Scene;
		}

		bool operator!=(const Entity& other) const {
			return !(*this == other);
		}
	private:
		entt::entity _Handle = entt::null;
		Scene* _Scene = nullptr;
	};
}