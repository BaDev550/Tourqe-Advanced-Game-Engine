#pragma once
#include "Shader.h"

#include <string>
#include <unordered_map>

namespace TARE {
	class ShaderLibrary
	{
	public:
		void LoadShader(const std::string_view& name, const std::string& shaderPath);
		void ReloadShader(const std::string_view& name);
		const TAGE::MEM::Ref<Shader>& GetShader(const std::string& name);
	private:
		std::unordered_map<std::string, TAGE::MEM::Ref<Shader>> _Shaders;
	};
}
