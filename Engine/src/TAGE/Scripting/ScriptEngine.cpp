#include "tagepch.h"
#include "ScriptEngine.h"
#include "TAGE/World/Scene/Scene.h"
#include "TAGE/World/Objects/Entity.h"

#include "TAGE/World/Components/ScriptingComponents.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/tabledefs.h"

#include "ScriptGlue.h"

namespace TAGE {

	static std::unordered_map<std::string, ScriptFieldType> s_ScriptFieldTypeMap = {
		{ "System.Boolean",     ScriptFieldType::Bool   },
		{ "System.Single",   ScriptFieldType::Float  },
		{ "System.Double",   ScriptFieldType::Double },

		{ "System.Byte",     ScriptFieldType::Byte  },
		{ "System.Char",     ScriptFieldType::Char  },
		{ "System.Int16",    ScriptFieldType::Short },
		{ "System.Int32",    ScriptFieldType::Int   },
		{ "System.Int64",    ScriptFieldType::Long  },

		{ "System.UInt32",   ScriptFieldType::UInt   },
		{ "System.UInt16",   ScriptFieldType::UShort },
		{ "System.UInt64",   ScriptFieldType::ULong  },

		{ "TAGE.Entity",   ScriptFieldType::Entity  },
		{ "TAGE.Vector2",  ScriptFieldType::Vector2 },
		{ "TAGE.Vector3",  ScriptFieldType::Vector3 },
		{ "TAGE.Vector4",  ScriptFieldType::Vector4 },
	};

	namespace Utils {

		static char* ReadBytes(const std::filesystem::path& filepath, uint* outsize) {
			std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

			if (!stream) {
				return nullptr;
			}

			std::streampos end = stream.tellg();
			stream.seekg(0, std::ios::beg);
			uint size = end - stream.tellg();

			if (size == 0) {
				return nullptr;
			}

			char* buffer = new char[size];
			stream.read((char*)buffer, size);
			stream.close();

			*outsize = size;
			return buffer;
		}

		static MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath) {
			uint filesize = 0;
			char* fileData = ReadBytes(assemblyPath, &filesize);

			MonoImageOpenStatus status;
			MonoImage* image = mono_image_open_from_data_full(fileData, filesize, 1, &status, 0);

			if (status != MONO_IMAGE_OK) {
				const char* errorMessage = mono_image_strerror(status);
				LOG_ERROR(errorMessage);
				return nullptr;
			}

			std::string pathString = assemblyPath.string();
			MonoAssembly* assembly = mono_assembly_load_from_full(image, pathString.c_str(), &status, 0);
			mono_image_close(image);

			delete[] fileData;
			return assembly;
		}

		void PrintAssemblyTypes(MonoAssembly* assembly)
		{
			MonoImage* image = mono_assembly_get_image(assembly);
			const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
			int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

			for (int32_t i = 0; i < numTypes; i++)
			{
				uint32_t cols[MONO_TYPEDEF_SIZE];
				mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

				const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
				const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

				LOG_TRACE("{}.{}", nameSpace, name);
			}
		}

		ScriptFieldType MonoTypeToScriptFieldType(MonoType* monoType) {
			std::string typeName = mono_type_get_name(monoType);

			auto it = s_ScriptFieldTypeMap.find(typeName);
			if (it == s_ScriptFieldTypeMap.end()) {
				LOG_ERROR("Unknown field type: {}", typeName);
				return ScriptFieldType::None;
			}

			return it->second;
		}

		const char* FieldTypeToString(ScriptFieldType type) {
			switch (type)
			{
			case ScriptFieldType::None:   return "<None>";

			case ScriptFieldType::Bool:   return "Bool";
			case ScriptFieldType::Float:  return "Float";
			case ScriptFieldType::Double: return "Double";

			case ScriptFieldType::Int:   return "Int";
			case ScriptFieldType::Byte:  return "Byte";
			case ScriptFieldType::Char:  return "Char";
			case ScriptFieldType::Short: return "Short";
			case ScriptFieldType::Long:  return "Long";

			case ScriptFieldType::UInt:   return "Unsigned Int";
			case ScriptFieldType::UShort: return "Unsigned Short";
			case ScriptFieldType::ULong:  return "Unsigned Long";

			case ScriptFieldType::Vector2: return "Vector2";
			case ScriptFieldType::Vector3: return "Vector3";
			case ScriptFieldType::Vector4: return "Vector4";
			case ScriptFieldType::Entity:  return "Entity";
			default:
				return "<Invalid>";
			}
			return "<Invalid>";
		}
	}

	struct ScriptEngineData {
		MonoDomain* RootDomain = nullptr;
		MonoDomain* AppDomain = nullptr;

		MonoAssembly* CoreAssembly = nullptr;
		MonoImage* CoreAssemblyImage = nullptr;

		MonoAssembly* AppAssembly = nullptr;
		MonoImage* AppAssemblyImage = nullptr;

		ScriptClass EntityClass;
		std::unordered_map<std::string, MEM::Ref<ScriptClass>> EntityClasses;
		std::unordered_map<UUID, MEM::Ref<ScriptInstance>> EntityInstances;

		Scene* SceneContext = nullptr;
	};

	static ScriptEngineData* s_Data;

	void ScriptEngine::Init()
	{
		s_Data = new ScriptEngineData();
		InitMono();
		LoadAssembly("Resources/Scripts/ScriptCore.dll");
		LoadAppAssembly("TestProject/Assets/Scripts/Binaries/Sandbox.dll");
		LoadAssemblyClasses();

		ScriptGlue::RegisterComponents();
		ScriptGlue::RegisterFunctions();

		s_Data->EntityClass = ScriptClass("TAGE", "Entity", true);
	}

	void ScriptEngine::Shutdown()
	{
		DestroyMono();
		delete s_Data;
		LOG_INFO("Script Engine Destroyed");
	}

	void ScriptEngine::LoadAssembly(const std::filesystem::path& filePath)
	{
		s_Data->AppDomain = mono_domain_create_appdomain("TAGEScriptRuntime", nullptr);
		mono_domain_set(s_Data->AppDomain, true);

		s_Data->CoreAssembly = Utils::LoadMonoAssembly(filePath);
		s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);
	}

	void ScriptEngine::LoadAppAssembly(const std::filesystem::path& filePath)
	{
		s_Data->AppAssembly = Utils::LoadMonoAssembly(filePath);
		s_Data->AppAssemblyImage = mono_assembly_get_image(s_Data->AppAssembly);
	}

	void ScriptEngine::OnRuntimeStart(Scene* scene)
	{
		s_Data->SceneContext = scene;
	}

	void ScriptEngine::OnRuntimeStop()
	{
		s_Data->SceneContext = nullptr;
		s_Data->EntityInstances.clear();
	}

	void ScriptEngine::OnCreateEntity(Entity entity)
	{
		const auto& sc = entity.GetComponent<ScriptComponent>();
		if (EntityClassExists(sc.Name)) {
			MEM::Ref<ScriptInstance> instance = MEM::MakeRef<ScriptInstance>(s_Data->EntityClasses[sc.Name], entity);
			s_Data->EntityInstances[entity.GetUUID()] = instance;
			instance->InvokeOnCreate();
		}
	}

	void ScriptEngine::OnUpdateEntity(Entity entity, float dt)
	{
		UUID entityID = entity.GetUUID();

		ASSERT(s_Data->EntityInstances.find(entityID) != s_Data->EntityInstances.end(), "Cannot find Class");
		MEM::Ref<ScriptInstance> instance = s_Data->EntityInstances[entityID];
		instance->InvokeOnUpdate(dt);
	}

	void ScriptEngine::OnFixedUpdateEntity(Entity entity, float dt)
	{
		UUID entityID = entity.GetUUID();

		ASSERT(s_Data->EntityInstances.find(entityID) != s_Data->EntityInstances.end(), "Cannot find Class");
		MEM::Ref<ScriptInstance> instance = s_Data->EntityInstances[entityID];
		instance->InvokeOnFixedUpdate(dt);
	}

	Scene* ScriptEngine::GetSceneContext() { return s_Data->SceneContext; }

	bool ScriptEngine::EntityClassExists(const std::string& fullClassName)
	{
		return s_Data->EntityClasses.find(fullClassName) != s_Data->EntityClasses.end();
	}

	std::unordered_map<std::string, MEM::Ref<ScriptClass>> ScriptEngine::GetEntityClasses()
	{
		return s_Data->EntityClasses;
	}

	MEM::Ref<ScriptInstance> ScriptEngine::GetEntityScriptInstance(UUID uuid)
	{
		auto it = s_Data->EntityInstances.find(uuid);
		if (it == s_Data->EntityInstances.end())
			return nullptr;

		return it->second;
	}

	void ScriptEngine::InitMono()
	{
		mono_set_assemblies_path("mono/lib");

		MonoDomain* rootDomain = mono_jit_init("TAGEJITRuntime");
		ASSERT(rootDomain, "Failed to init mono");
		s_Data->RootDomain = rootDomain;
	}


	void ScriptEngine::DestroyMono()
	{
		//mono_domain_unload(s_Data->AppDomain);
		s_Data->AppDomain = nullptr;

		//mono_jit_cleanup(s_Data->RootDomain);
		s_Data->RootDomain = nullptr;
	}

	MonoObject* ScriptEngine::InstantiateClass(MonoClass* monoClass)
	{
		MonoObject* instance = mono_object_new(s_Data->AppDomain, monoClass);
		mono_runtime_object_init(instance);
		return instance;
	}

	void ScriptEngine::LoadAssemblyClasses()
	{
		s_Data->EntityClasses.clear();

		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(s_Data->AppAssemblyImage, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
		MonoClass* entityClass = mono_class_from_name(s_Data->CoreAssemblyImage, "TAGE", "Entity");

		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(s_Data->AppAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(s_Data->AppAssemblyImage, cols[MONO_TYPEDEF_NAME]);
			std::string fullName;
			if (strlen(nameSpace) != 0)
				fullName = fmt::format("{}.{}", nameSpace, name);
			else
				fullName = name;

			MonoClass* monoClass = mono_class_from_name(s_Data->AppAssemblyImage, nameSpace, name);

			if (monoClass == entityClass)
				continue;

			bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false);

			if (!isEntity)
				continue;

			MEM::Ref<ScriptClass> scriptClass = MEM::MakeRef<ScriptClass>(nameSpace, name);
			s_Data->EntityClasses[fullName] = scriptClass;

			void* it = nullptr;
			while (MonoClassField* field = mono_class_get_fields(monoClass, &it)) {
				const char* fieldName = mono_field_get_name(field);
				uint flags = mono_field_get_flags(field);
				if (flags & FIELD_ATTRIBUTE_PUBLIC){
					MonoType* type = mono_field_get_type(field);
					ScriptFieldType fieldType = Utils::MonoTypeToScriptFieldType(type);
					LOG_WARN("   {} - ({})", fieldName, Utils::FieldTypeToString(fieldType));

					scriptClass->_Fields[fieldName] = { fieldName, fieldType, field };
				}
			}
		}
	}

	MonoImage* ScriptEngine::GetCoreAssemblyImage()
	{
		return s_Data->CoreAssemblyImage;
	}

	ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className, bool isCore) : _ClassNamespace(classNamespace), _ClassName(className) {
		_MonoClass = mono_class_from_name(isCore ? s_Data->CoreAssemblyImage : s_Data->AppAssemblyImage, classNamespace.c_str(), className.c_str());
	}

	MonoObject* ScriptClass::Instantiate() { 
		return ScriptEngine::InstantiateClass(_MonoClass); 
	}

	MonoObject* ScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** param)
	{
		return mono_runtime_invoke(method, instance, param, nullptr);;
	}

	MonoMethod* ScriptClass::GetMethod(const std::string& methodName, int parameterCount) {
		return mono_class_get_method_from_name(_MonoClass, methodName.c_str(), parameterCount);
	}

	ScriptInstance::ScriptInstance(const MEM::Ref<ScriptClass>& scriptClass, Entity entity)
		: _ScriptClass(scriptClass)
	{
		_Instance = scriptClass->Instantiate();
		_Constructor = s_Data->EntityClass.GetMethod(".ctor", 1);
		_OnCreateMethod = scriptClass->GetMethod("OnCreate", 0);
		_OnUpdateMethod = scriptClass->GetMethod("OnUpdate", 1);
		_OnFixedUpdateMethod = scriptClass->GetMethod("OnFixedUpdate", 1);

		{
			UUID entityID = entity.GetUUID();
			void* param = &entityID;
			_ScriptClass->InvokeMethod(_Instance, _Constructor, &param);
		}
	}

	void ScriptInstance::InvokeOnCreate()
	{
		if (_OnCreateMethod)
			_ScriptClass->InvokeMethod(_Instance, _OnCreateMethod);
	}

	void ScriptInstance::InvokeOnUpdate(float dt)
	{
		if (_OnUpdateMethod) {
			void* param = &dt;
			_ScriptClass->InvokeMethod(_Instance, _OnUpdateMethod, &param);
		}
	}

	void ScriptInstance::InvokeOnFixedUpdate(float dt)
	{
		if (_OnFixedUpdateMethod) {
			void* param = &dt;
			_ScriptClass->InvokeMethod(_Instance, _OnFixedUpdateMethod, &param);
		}
	}

	bool ScriptInstance::GetFieldValueInternal(const std::string& name, void* buffer)
	{
		const auto& fields = _ScriptClass->GetFields();
		auto it = fields.find(name);
		if (it == fields.end())
			return false;

		const ScriptField& field = it->second;
		mono_field_get_value(_Instance, field.ClassField, buffer);
		return true;
	}

	bool ScriptInstance::SetFieldValueInternal(const std::string& name, const void* value)
	{
		const auto& fields = _ScriptClass->GetFields();
		auto it = fields.find(name);
		if (it == fields.end())
			return false;

		const ScriptField& field = it->second;
		mono_field_set_value(_Instance, field.ClassField, (void*)value);
		return true;
	}
}