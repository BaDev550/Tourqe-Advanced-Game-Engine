#pragma once

#include "Shader.h"
#include "TAGE/Utilities/Memory.h"
#include <unordered_map>

namespace TAGE::Renderer {
	class ShaderLibrary {
	public:
		ShaderLibrary() = default;
		~ShaderLibrary() {
			for (auto& pair : _shaders) {
				pair.second->DeleteShader();
			}
			_shaders.clear();
		};

		static MEM::Ref<Shader> Add(const std::string& name, const char* vertexPath, const char* fragmentPath);
		static void Remove(const std::string& name) {
			_shaders.erase(name);
		}
		static MEM::Ref<Shader> Get(const std::string& name) {
			return _shaders.at(name);
		}
	private:
		static inline std::unordered_map<std::string, MEM::Ref<Shader>> _shaders;
	};
}