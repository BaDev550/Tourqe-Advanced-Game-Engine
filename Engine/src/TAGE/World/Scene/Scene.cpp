#include "tagepch.h"
#include "Scene.h"

namespace TAGE {
	Scene::Scene(const std::string& name) : _Name(name)
	{
	}

	Object& Scene::CreateObject(const std::string& name)
	{
		entt::entity handle = _Registry.create();
		MEM::Ref<Object> NewObject = MEM::MakeRef<Object>(handle, &_Registry, name);
		Object& objRef = *NewObject;
		_Objects[handle] = std::move(NewObject);
		_NamedObjects[name] = handle;
		return objRef;
	}

	bool Scene::DestroyObject(Object* object)
	{
		if (!object)
			return false;

		entt::entity handle = object->GetHandle();
		std::string name = object->GetName();

		object->Destroy();
		_Objects.erase(handle);
		_NamedObjects.erase(name);

		return true;
	}

	void Scene::Update(float DeltaTime, SystemUpdateType updateType)
	{
		for (const auto& system : _Systems)
			system->Update(updateType, DeltaTime);
	}

	Object* Scene::GetObjectByName(const std::string& name)
	{
		auto it = _NamedObjects.find(name);
		if (it != _NamedObjects.end()) {
			auto objIt = _Objects.find(it->second);
			if (objIt != _Objects.end())
				return objIt->second.get();
		}
		return nullptr;
	}
	Object* Scene::GetObjectByID(const entt::entity& ID)
	{
		auto objIt = _Objects.find(ID);
		if (objIt != _Objects.end())
			return objIt->second.get();
		return nullptr;
	}
}