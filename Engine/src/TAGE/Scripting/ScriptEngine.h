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
			static_assert(sizeof(T) <= 16, "Type too large");
			return *(T*)_Buffer;
		}

		template<typename T>
		void SetValue(T value) {
			static_assert(sizeof(T) <= 16, "Type too large");
			memcpy(_Buffer, &value, sizeof(T));
		}
	private:
		uint8 _Buffer[16];

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
			static_assert(sizeof(T) <= 16, "Type too large");

			bool success = GetFieldValueInternal(name, s_FieldValueBuffer);
			if (!success)
				return T();

			return *(T*)s_FieldValueBuffer;
		}

		template<typename T>
		void SetFieldValue(const std::string& name, T value) {
			static_assert(sizeof(T) <= 16, "Type too large");

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
		static bool IsInitialized() { return s_Initialized; }
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

		static inline bool s_Initialized = false;
	};

	namespace Utils {
		inline const char* ScriptFieldTypeToString(ScriptFieldType type) {
			switch (type)
			{
			case TAGE::ScriptFieldType::None:   return "None";
			case TAGE::ScriptFieldType::Float:  return "Float";
			case TAGE::ScriptFieldType::Double: return "Double";
			case TAGE::ScriptFieldType::Byte:   return "Byte";
			case TAGE::ScriptFieldType::Short:  return "Short";
			case TAGE::ScriptFieldType::Int:    return "Int";
			case TAGE::ScriptFieldType::Long:   return "Long";
			case TAGE::ScriptFieldType::Bool:   return "Bool";
			case TAGE::ScriptFieldType::Char:   return "Char";
			case TAGE::ScriptFieldType::UShort: return "UShort";
			case TAGE::ScriptFieldType::UInt:   return "UInt";
			case TAGE::ScriptFieldType::ULong:  return "ULong";
			case TAGE::ScriptFieldType::Vector2:return "Vector2";
			case TAGE::ScriptFieldType::Vector3:return "Vector3";
			case TAGE::ScriptFieldType::Vector4:return "Vector4";
			case TAGE::ScriptFieldType::Entity: return "Entity";
			}
			ASSERT(false, "Unknown Field Type");
			return "None";
		}

		inline ScriptFieldType StringToScriptFieldType(std::string_view type) {
			if (type == "Float")   return ScriptFieldType::Float;
			if (type == "Double")  return ScriptFieldType::Double;
			if (type == "Byte")    return ScriptFieldType::Byte;
			if (type == "Short")   return ScriptFieldType::Short;
			if (type == "Int")     return ScriptFieldType::Int;
			if (type == "Long")    return ScriptFieldType::Long;
			if (type == "Bool")    return ScriptFieldType::Bool;
			if (type == "Char")    return ScriptFieldType::Char;
			if (type == "UShort")  return ScriptFieldType::UShort;
			if (type == "UInt")    return ScriptFieldType::UInt;
			if (type == "ULong")   return ScriptFieldType::ULong;
			if (type == "Vector2") return ScriptFieldType::Vector2;
			if (type == "Vector3") return ScriptFieldType::Vector3;
			if (type == "Vector4") return ScriptFieldType::Vector4;
			if (type == "Entity")  return ScriptFieldType::Entity;

			ASSERT(false, "Unknown Field Type");
			return ScriptFieldType::None;
		}
	}
}