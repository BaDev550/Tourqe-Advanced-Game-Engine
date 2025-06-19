#pragma once

#include "entt/entt.hpp"
#include "TAGE/World/Components/BaseComponents.h"

namespace TAGE {
	class Object {
	public:
		Object(entt::entity& handle, entt::registry* registry, const std::string& objectName = "UNDEFINED") : _Handle(handle), _Registry(registry) {
			_Identity = AddComponent<IdentityComponent>(objectName);
			_Transform = &AddComponent<TransformComponent>();
		}

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args) {
			if (!HasComponent<T>()) {
				return _Registry->emplace<T>(_Handle, std::forward<Args>(args)...);
			}
			return _Registry->get<T>(_Handle);
		}

		template<typename T>
		T* GetComponent() { 
			if (HasComponent<T>())
				return &_Registry->get<T>(_Handle);
			else
				return nullptr;
		}

		template<typename T>
		void RemoveComponent() {
			if (HasComponent<T>())
				_Registry->remove<T>(_Handle);
		}

		template<typename T>
		bool HasComponent() const { return _Registry->any_of<T>(_Handle); }
		
		void Destroy() {
			if (_Registry && _Handle != entt::null)
				_Registry->destroy(_Handle);
			_Handle = entt::null;
			_Registry = nullptr;
		}

		entt::entity GetHandle() const { return _Handle; }
		std::string GetName() const { return _Identity.Name; }

		TransformComponent& GetTransform() { return *_Transform; }

        glm::mat4& GetTransformMatrix() const {  
           glm::mat4 translation = glm::translate(glm::mat4(1.0f), _Transform->Position);  
           glm::mat4 rotation = glm::mat4_cast(_Transform->Rotation);
           glm::mat4 scale = glm::scale(glm::mat4(1.0f), _Transform->Scale);  
           return translation * rotation * scale;  
        }
	private:
		entt::entity _Handle = entt::null;
		entt::registry* _Registry = nullptr;
		IdentityComponent _Identity;
		TransformComponent* _Transform;
	};
}