#pragma once
#include "TAGE/World/Systems/System_Base.h"
#include "TAGE/World/Objects/Object.h"
#include <vector>

namespace TAGE {
	class Scene
	{
	public:
		Scene(const std::string& name);
		Object& CreateObject(const std::string& name);
		bool DestroyObject(Object* object);

		void Update(float DeltaTime, SystemUpdateType updateType = SystemUpdateType::UPDATE);

		Object* GetObjectByName(const std::string& name);
		Object* GetObjectByID(const entt::entity& ID);

		std::unordered_map<entt::entity, MEM::Ref<Object>>& GetObjects() { return _Objects; }
		entt::registry& GetRegistry() { return _Registry; }
		std::string& GetName() { return _Name; }

		void AddSystem(MEM::Ref<System> system) { system->SetActiveScene(this); _Systems.push_back(system); }
	private:
		std::unordered_map<entt::entity, MEM::Ref<Object>> _Objects;
		std::unordered_map<std::string, entt::entity> _NamedObjects;
		std::vector<MEM::Ref<System>> _Systems;
		entt::registry _Registry;
		std::string _Name;
	};
}