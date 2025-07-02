#pragma once

#include "TAGE/World/Scene/Scene.h"

namespace TAGE {
	class Entity;
	class Prefab
	{
	public:
		Prefab();
		~Prefab();

		void Create(Entity entity);
	private:
		Entity CreatePrefabFromEntity(Entity entity);
	private:
		MEM::Ref<Scene> _Scene;
		Entity _Entity;

		friend class Scene;
	};
}