#pragma once

#include "Shader.h"
#include "TAGE/Utilities/Memory.h"
#include <unordered_map>

namespace TARE {
	class ShaderLibrary {
	public:
		ShaderLibrary() = default;
		~ShaderLibrary() {
			for (auto& pair : _shaders) {
				pair.second->DestroyProgram();
			}
			_shaders.clear();
		};

		static TAGE::MEM::Ref<Shader> Add(const std::string& name, const char* vertexPath, const char* fragmentPath, const char* geometryPath = "0");
		static void Remove(const std::string& name) { _shaders[name]->DestroyProgram(); _shaders.erase(name); }
		static TAGE::MEM::Ref<Shader> Get(const std::string& name) { return _shaders.at(name); }
		static std::unordered_map<std::string, TAGE::MEM::Ref<Shader>>& GetShaders() { return _shaders; }
	private:
		static inline std::unordered_map<std::string, TAGE::MEM::Ref<Shader>> _shaders;
	};
}