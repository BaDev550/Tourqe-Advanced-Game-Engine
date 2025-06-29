#pragma once

#include <filesystem>

extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
}

namespace TAGE {
	class ScriptEngine
	{
	public:
		static void Init();
		static void Shutdown();

		static void LoadAssembly(const std::filesystem::path& filePath);
	private:
		static void InitMono();
		static void DestroyMono();

		static MonoObject* InstantiateClass(MonoClass* monoClass);
	};
}