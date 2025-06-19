#include "tagepch.h"
#include "Shader.h"
#include "ShaderPreprocessor.h"
#include "TARE/Common/RenderAPI.h"

#include "Platform/OpenGL/OpenGL_Shader.h"

namespace TARE
{
	Shader::~Shader()
	{
		DestroyProgram();
	}

	void Shader::LoadShader(const char* vertexPath, const char* fragmentPath)
	{
		std::ifstream vertexFile(vertexPath);
		std::ifstream fragmentFile(fragmentPath);
		if (!vertexFile.is_open() || !fragmentFile.is_open()) {
			LOG_CRITICAL("Failed to open shader files: {} or {}", vertexPath, fragmentPath);
			return;
		}
		std::unordered_set<std::string> included;

		std::stringstream vertexStream, fragmentStream;
		vertexStream << vertexFile.rdbuf();
		fragmentStream << fragmentFile.rdbuf();
		std::string vertexCode = ShaderPreprocessor::Preprocess(vertexPath);
		std::string fragmentCode = ShaderPreprocessor::Preprocess(fragmentPath);
		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();
		unsigned int vertex = 0, fragment = 0;

		CompileShader(vertex, vShaderCode, "VERTEX");
		CompileShader(fragment, fShaderCode, "FRAGMENT");
		CompileProgram(fragment, vertex);

		DeleteShader(vertex);
		DeleteShader(fragment);
		vertexFile.close();
		fragmentFile.close();
	}

	TAGE::MEM::Ref<Shader> TARE::Shader::Create(const char* vertexPath, const char* fragmentPath)
	{
		switch (RenderAPI::GetRenderAPI())
		{
		case RAPI::UNDEFINED:
		case RAPI::OPENGL: return TAGE::MEM::MakeRef<OpenGL_Shader>(vertexPath, fragmentPath);
		case RAPI::DIRECTX11:
		case RAPI::VULKAN:
		default:
			ASSERT_NOMSG(false);
			break;
		}
		return nullptr;
	}
}