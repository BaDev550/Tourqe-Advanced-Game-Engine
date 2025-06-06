#include "tagepch.h"
#include "ShaderLibrary.h"
#include "TAGE/Application/Application.h"

namespace TAGE::Renderer {

	MEM::Ref<Shader> ShaderLibrary::Add(const std::string& name, const char* vertexPath, const char* fragmentPath) {
		MEM::Ref<Shader> shader = MEM::MakeRef<Shader>(vertexPath, fragmentPath);
		_shaders[name] = shader;
		return shader;
	}

}