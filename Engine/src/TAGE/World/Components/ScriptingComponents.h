#pragma once
#include "TAGE/World/Objects/ScriptableEntity.h"

namespace TAGE {
	struct NativeScriptComponent {
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity* (*InstantiateScript)();
		void (*DestroyScript)(NativeScriptComponent*);

		template<typename T>
		void Bind() {
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
		}

		NativeScriptComponent(const NativeScriptComponent&) = default;
	};

	struct ScriptComponent {
		std::string Name;

		ScriptComponent() = default;
		ScriptComponent(const std::string& name) : Name(name) {}
		ScriptComponent(const ScriptComponent&) = default;
	};
}