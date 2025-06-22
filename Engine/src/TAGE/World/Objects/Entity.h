#pragma once

#include "TAGE/World/Scene/Scene.h"
#include "TAGE/Utilities/Logger.h"
#include "entt/entt.hpp"

namespace TAGE {
	class Entity {
	public:
		Entity() = default;
		Entity(entt::entity& handle, Scene* scene) : _Handle(handle), _Scene(scene) {}
		Entity(const Entity& other) = default;

		template<typename T>
		bool HasComponent() const { return _Scene->GetRegistry().any_of<T>(_Handle); }

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args) {
			if (!HasComponent<T>()) {
				return _Scene->GetRegistry().emplace<T>(_Handle, std::forward<Args>(args)...);
			}
			return _Scene->GetRegistry().get<T>(_Handle);
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
		
		void Destroy() {
			if (_Handle != entt::null)
				_Scene->GetRegistry().destroy(_Handle);
			_Handle = entt::null;
		}

		operator entt::entity() const { return _Handle; }
		operator bool() const { return _Handle != entt::null; }
	private:
		entt::entity _Handle = entt::null;
		Scene* _Scene = nullptr;
	};
}