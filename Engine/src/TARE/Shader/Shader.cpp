#include "tagepch.h"
#include "Shader.h"

#include "Platform/OpenGL/OpenGL_Shader.h"

namespace TARE {
	TAGE::MEM::Ref<Shader> Shader::Create(const std::string& shaderPath)
	{
		switch (RenderAPI::GetGrapichAPI())
		{
		case GrapichAPI::OpenGL: return TAGE::MEM::MakeRef<OpenGL::OpenGL_Shader>(shaderPath);
		default:
			break;
		}
	}
}