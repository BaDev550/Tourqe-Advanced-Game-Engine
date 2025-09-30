#include "tagepch.h"
#include "ShaderLibrary.h"

namespace TARE {
	void ShaderLibrary::LoadShader(const std::string_view& name, const std::string& shaderPath) {
		_Shaders[name.data()] = Shader::Create(shaderPath);
	}

	void ShaderLibrary::ReloadShader(const std::string_view& name) {
		ASSERT("Coudnt find the wanted shader", _Shaders.find(name.data()) == _Shaders.end());
		_Shaders[name.data()]->Reload();
	}

	const TAGE::MEM::Ref<Shader>& ShaderLibrary::GetShader(const std::string& name) {
		return _Shaders[name];
	}
}