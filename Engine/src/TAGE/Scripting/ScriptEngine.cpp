#include "tagepch.h"
#include "ScriptEngine.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"

#include "ScriptGlue.h"

namespace TAGE {
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
	}

	struct ScriptEngineData {
		MonoDomain* RootDomain = nullptr;
		MonoDomain* AppDomain = nullptr;

		MonoAssembly* CoreAssembly = nullptr;
		MonoImage* CoreAssemblyImage = nullptr;

		MonoClass* EntityClass = nullptr;
	};

	static ScriptEngineData* s_Data;

	class ScriptClass {
	public:
		ScriptClass() {}
	};

	void ScriptEngine::Init()
	{
		s_Data = new ScriptEngineData();
		InitMono();
		LoadAssembly("Resources/Scripts/ScriptCore.dll");

		ScriptGlue::RegisterFunctions();
		s_Data->EntityClass = mono_class_from_name(s_Data->CoreAssemblyImage, "TAGE", "Entity");
		MonoObject* instance = InstantiateClass(s_Data->EntityClass);

		MonoMethod* printMsgFunction = mono_class_get_method_from_name(s_Data->EntityClass, "PrintMessage", 0);
		mono_runtime_invoke(printMsgFunction, instance, nullptr, nullptr);
		MonoMethod* printIntFuntion = mono_class_get_method_from_name(s_Data->EntityClass, "PrintInt", 1);
		int value = 5;
		void* params[1] = { &value };
		mono_runtime_invoke(printIntFuntion, instance, params, nullptr);
		MonoString* monoString = mono_string_new(s_Data->AppDomain, "Hello World From C++");
		MonoMethod* printMsgWithParamFunction = mono_class_get_method_from_name(s_Data->EntityClass, "Print", 1);
		void* stringParam = monoString;
		mono_runtime_invoke(printMsgWithParamFunction, instance, &stringParam, nullptr);
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
#if 0
		Utils::PrintAssemblyTypes(s_Data->CoreAssembly);
#endif

		s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);
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
}