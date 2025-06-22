#pragma once
#include "Entity.h"

namespace TAGE {

	class ScriptableEntity
	{
	public:
		virtual ~ScriptableEntity() = default;

		template<typename T>
		T& GetComponent()
		{
			return _Entity.GetComponent<T>();
		}

	protected:
		virtual void OnCreate() {}
		virtual void OnDestroy() {}
		virtual void OnUpdate(float DeltaTime) {}
	private:
		Entity _Entity;
		friend class Scene;
	};

}