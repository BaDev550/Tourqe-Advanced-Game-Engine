#include "tagepch.h"
#include "Prefab.h"
#include "TAGE/World/Objects/Entity.h"

namespace TAGE {
	Prefab::Prefab()
	{
		_Scene = Scene::CreateEmpty();
	}

	Prefab::~Prefab()
	{
	}

	void Prefab::Create(Entity entity)
	{
		_Scene = Scene::CreateEmpty();
		_Entity = CreatePrefabFromEntity(entity);
	}

	Entity Prefab::CreatePrefabFromEntity(Entity entity)
	{
		std::string prefabName = entity.GetName() + "_prefab";
		
		Entity newEntity = _Scene->CreateEntity(prefabName);

		entity._Scene->CopyComponentIfExists<TransformComponent>(newEntity, entity._Scene->_Registry, entity);
		entity._Scene->CopyComponentIfExists<MeshComponent>(newEntity,      entity._Scene->_Registry, entity);
		entity._Scene->CopyComponentIfExists<LightComponent>(newEntity,     entity._Scene->_Registry, entity);
		entity._Scene->CopyComponentIfExists<SkyboxComponent>(newEntity,    entity._Scene->_Registry, entity);
		entity._Scene->CopyComponentIfExists<ScriptComponent>(newEntity,    entity._Scene->_Registry, entity);
		entity._Scene->CopyComponentIfExists<CameraComponent>(newEntity,    entity._Scene->_Registry, entity);
		entity._Scene->CopyComponentIfExists<RigidBodyComponent>(newEntity, entity._Scene->_Registry, entity);
		entity._Scene->CopyComponentIfExists<ColliderComponent>(newEntity,  entity._Scene->_Registry, entity);
		for (auto childId : entity.Children()) {
			Entity childDuplicate = CreatePrefabFromEntity(entity._Scene->GetEntityByUUID(childId));

			childDuplicate.SetParentUUID(newEntity.GetUUID());
			newEntity.Children().push_back(childDuplicate.GetUUID());
		}

		return newEntity;
	}
}