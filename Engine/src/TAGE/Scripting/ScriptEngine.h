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
	typedef struct _MonoClassField MonoClassField;
}

namespace TAGE {
	enum class ScriptFieldType : uint8 {
		None = 0,
		Float, Double,
		Byte, Short, Int, Long, Bool, Char,
		UShort, UInt, ULong,

		Vector2, Vector3, Vector4,
		Entity
	};

	struct ScriptField {
		std::string Name;
		ScriptFieldType Type;
		MonoClassField* ClassField;
	};

	struct ScriptFieldInstance {
	public:
		ScriptField Field;

		ScriptFieldInstance() {
			memset(_Buffer, 0, sizeof(_Buffer));
		}

		template<typename T>
		T GetValue() {
			static_assert(sizeof(T) <= 8, "Type too large");
			return *(T*)_Buffer;
		}

		template<typename T>
		void SetValue(T value) {
			static_assert(sizeof(T) <= 8, "Type too large");
			memcpy(_Buffer, &value, sizeof(T));
		}
	private:
		uint8 _Buffer[8];

		friend class ScriptEngine;
	};

	using ScriptFieldMap = std::unordered_map<std::string, ScriptFieldInstance>;

	class ScriptClass {
	public:
		ScriptClass() = default;
		ScriptClass(const std::string& classNamespace, const std::string& className, bool isCore = false);

		MonoObject* Instantiate();
		MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** param = nullptr);
		MonoMethod* GetMethod(const std::string& methodName, int parameterCount);

		const std::map<std::string, ScriptField>& GetFields() { return _Fields; }
	private:
		std::string _ClassNamespace;
		std::string _ClassName;
		std::map<std::string, ScriptField> _Fields;
		MonoClass* _MonoClass = nullptr;

		friend class ScriptEngine;
	};

	class Scene;
	class Entity;

	class ScriptInstance {
	public:
		ScriptInstance(const MEM::Ref<ScriptClass>& scriptClass, Entity entity);

		void InvokeOnCreate();
		void InvokeOnUpdate(float dt);
		void InvokeOnFixedUpdate(float dt);

		MEM::Ref<ScriptClass> GetScriptClass() { return _ScriptClass; }

		template<typename T>
		T GetFieldValue(const std::string& name) {
			static_assert(sizeof(T) <= 8, "Type too large");

			bool success = GetFieldValueInternal(name, s_FieldValueBuffer);
			if (!success)
				return T();

			return *(T*)s_FieldValueBuffer;
		}

		template<typename T>
		void SetFieldValue(const std::string& name, T value) {
			static_assert(sizeof(T) <= 8, "Type too large");

			SetFieldValueInternal(name, &value);
		}
	private:
		bool GetFieldValueInternal(const std::string& name, void* buffer);
		bool SetFieldValueInternal(const std::string& name, const void* value);
	private:
		MEM::Ref<ScriptClass> _ScriptClass;
		MonoObject* _Instance = nullptr;

		MonoMethod* _Constructor = nullptr;
		MonoMethod* _OnCreateMethod = nullptr;
		MonoMethod* _OnUpdateMethod = nullptr;
		MonoMethod* _OnFixedUpdateMethod = nullptr;

		inline static char s_FieldValueBuffer[8];

		friend class ScriptEngine;
	};

	class ScriptEngine
	{
	public:
		static void Init();
		static void Shutdown();

		static void LoadAssembly(const std::filesystem::path& filePath);
		static void LoadAppAssembly(const std::filesystem::path& filePath);

		static void OnRuntimeStart(Scene* scene);
		static void OnRuntimeStop();
		static void OnCreateEntity(Entity entity);
		static void OnUpdateEntity(Entity entity, float dt);
		static void OnFixedUpdateEntity(Entity entity, float dt);

		static Scene* GetSceneContext();
		static MonoImage* GetCoreAssemblyImage();
		static bool EntityClassExists(const std::string& fullClassName);
		static std::unordered_map<std::string, MEM::Ref<ScriptClass>> GetEntityClasses();
		static MEM::Ref<ScriptClass> GetEntityClass(const std::string& name);
		static ScriptFieldMap& GetScriptFieldMap(Entity entity);
		static MEM::Ref<ScriptInstance> GetEntityScriptInstance(UUID uuid);
	private:
		static void InitMono();
		static void DestroyMono();

		static MonoObject* InstantiateClass(MonoClass* monoClass);
		static void LoadAssemblyClasses();

		friend class ScriptClass;
		friend class ScriptGlue;
	};
}