#pragma once

namespace TAGE {
	template<typename T>
	bool Entity::HasComponent() const { return _Scene->GetRegistry().any_of<T>(_Handle); }

	template<typename T, typename... Args>
	T& Entity::AddComponent(Args&&... args) {
		ASSERT(!HasComponent<T>(), "Entity already has this component");
		T& component = _Scene->GetRegistry().emplace<T>(_Handle, std::forward<Args>(args)...);
		_Scene->OnComponentAdded(*this, component);

		return component;
	}

	template<typename T, typename... Args>
	T& Entity::AddOrReplaceComponent(Args&&... args) {
		T& component = _Scene->GetRegistry().emplace_or_replace<T>(_Handle, std::forward<Args>(args)...);
		_Scene->OnComponentAdded(*this, component);
		return component;
	}

	template<typename T>
	T& Entity::GetComponent() {
		ASSERT_NOMSG(HasComponent<T>());
		return _Scene->GetRegistry().get<T>(_Handle);
	}

	template<typename T>
	const T& Entity::GetComponent() const {
		ASSERT_NOMSG(HasComponent<T>());
		return _Scene->GetRegistry().get<T>(_Handle);
	}

	template<typename T>
	void Entity::RemoveComponent() {
		ASSERT_NOMSG(HasComponent<T>());
		_Scene->GetRegistry().remove<T>(_Handle);
	}
}