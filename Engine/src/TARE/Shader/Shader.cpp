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

	void Shader::LoadShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
	{
		bool hasGeometry = geometryPath[0] != '0';
		_VertexPath = vertexPath;
		_FragmentPath = fragmentPath;
		_GeometryPath = geometryPath;

		std::ifstream vertexFile(vertexPath);
		std::ifstream fragmentFile(fragmentPath);
		std::ifstream geometryFile;
		if (hasGeometry) {
			geometryFile.open(geometryPath);
		}

		if (!vertexFile.is_open() || !fragmentFile.is_open() || (hasGeometry && !geometryFile.is_open())) {
			LOG_CRITICAL("Failed to open shader files: {} or {} or {}", vertexPath, fragmentPath, geometryPath ? geometryPath : "null");
			return;
		}

		std::string vertexCode = ShaderPreprocessor::Preprocess(vertexPath);
		std::string fragmentCode = ShaderPreprocessor::Preprocess(fragmentPath);
		std::string geometryCode;
		if (hasGeometry)
			geometryCode = ShaderPreprocessor::Preprocess(geometryPath);

		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();
		const char* gShaderCode = hasGeometry ? geometryCode.c_str() : nullptr;

		uint vertex = 0, fragment = 0, geometry = 0;
		CompileShader(vertex, vShaderCode, "VERTEX");
		CompileShader(fragment, fShaderCode, "FRAGMENT");
		if (hasGeometry)
			CompileShader(geometry, gShaderCode, "GEOMETRY");

		CompileProgram(vertex, fragment, hasGeometry ? &geometry : nullptr);

		DeleteShader(vertex);
		DeleteShader(fragment);
		if (hasGeometry)
			DeleteShader(geometry);
	}

	bool Shader::Reload()
	{
		DestroyProgram();
		try {
			LOG_INFO("Reloading shader: {}", _VertexPath);
			LoadShader(_VertexPath.c_str(), _FragmentPath.c_str(), _GeometryPath.c_str());
			return true;
		}
		catch (const std::ifstream::failure& e) {
			LOG_ERROR("Failed to reload shader file: {}", e.what());
			return false;
		}
	}

	TAGE::MEM::Ref<Shader> TARE::Shader::Create(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
	{
		switch (RenderAPI::GetRenderAPI())
		{
		case RAPI::UNDEFINED:
		case RAPI::OPENGL: return TAGE::MEM::MakeRef<OpenGL_Shader>(vertexPath, fragmentPath, geometryPath);
		case RAPI::DIRECTX11:
		case RAPI::VULKAN:
		default:
			ASSERT_NOMSG(false);
			break;
		}
		return nullptr;
	}
}