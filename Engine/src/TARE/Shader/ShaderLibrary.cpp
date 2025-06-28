#include "tagepch.h"
#include "ShaderLibrary.h"
#include "TAGE/Application/Application.h"
#include "TARE/Common/RenderAPI.h"

namespace TARE {

	static const char* GetExtension() {
		switch (RenderAPI::GetRenderAPI())
		{
		case RAPI::UNDEFINED:
		case RAPI::OPENGL: return ".glsl";
		case RAPI::DIRECTX11: return ".hlsl";
		case RAPI::VULKAN:
		default:
			ASSERT_NOMSG(false);
			break;
		}
		return nullptr;
	}

	TAGE::MEM::Ref<Shader> ShaderLibrary::Add(const std::string& name, const char* vertexPath, const char* fragmentPath, const char* geometryPath) {
		std::string vertex = std::string(vertexPath) + GetExtension();
		std::string fragment = std::string(fragmentPath) + GetExtension();
		std::string geometry = std::string(geometryPath) + GetExtension();

		TAGE::MEM::Ref<Shader> shader = Shader::Create(vertex.c_str(), fragment.c_str(), geometry.c_str());
		_shaders[name] = shader;
		return shader;
	}

}