#pragma once

#include <filesystem>
#include "string"
#include "TAGE/Utilities/Memory.h"

extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoImage MonoImage;
}

namespace TAGE {
	class ScriptClass {
	public:
		ScriptClass() = default;
		ScriptClass(const std::string& classNamespace, const std::string& className);

		MonoObject* Instantiate();
		MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** param = nullptr);
		MonoMethod* GetMethod(const std::string& methodName, int parameterCount);
	private:
		std::string _ClassNamespace;
		std::string _ClassName;
		MonoClass* _MonoClass = nullptr;
	};

	class Scene;
	class Entity;

	class ScriptInstance {
	public:
		ScriptInstance(const MEM::Ref<ScriptClass>& scriptClass, Entity entity);

		void InvokeOnCreate();
		void InvokeOnUpdate(float dt);
		void InvokeOnFixedUpdate(float dt);
	private:
		MEM::Ref<ScriptClass> _ScriptClass;
		MonoObject* _Instance = nullptr;

		MonoMethod* _Constructor = nullptr;
		MonoMethod* _OnCreateMethod = nullptr;
		MonoMethod* _OnUpdateMethod = nullptr;
		MonoMethod* _OnFixedUpdateMethod = nullptr;
	};

	class ScriptEngine
	{
	public:
		static void Init();
		static void Shutdown();

		static void LoadAssembly(const std::filesystem::path& filePath);
		static void OnRuntimeStart(Scene* scene);
		static void OnRuntimeStop();
		static void OnCreateEntity(Entity entity);
		static void OnUpdateEntity(Entity entity, float dt);
		static void OnFixedUpdateEntity(Entity entity, float dt);

		static Scene* GetSceneContext();
		static MonoImage* GetCoreAssemblyImage();
		static bool EntityClassExists(const std::string& fullClassName);
		static std::unordered_map<std::string, MEM::Ref<ScriptClass>> GetEntityClasses();
	private:
		static void InitMono();
		static void DestroyMono();

		static MonoObject* InstantiateClass(MonoClass* monoClass);
		static void LoadAssemblyClasses(MonoAssembly* assembly);

		friend class ScriptClass;
		friend class ScriptGlue;
	};
}