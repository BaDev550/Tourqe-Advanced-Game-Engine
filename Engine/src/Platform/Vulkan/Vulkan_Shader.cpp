#include "tagepch.h"
#include <TAGE/Application/Application.h>
#include "Vulkan_Shader.h"
#include "Vulkan_ShaderCompiler.h"
#include "Vulkan_RenderContext.h"
#include <vulkan/vulkan.h>

namespace TARE {
	static std::string ReadFileText(const std::string& filepath) {
		std::ifstream file(filepath);
		if (!file.is_open()) {
			LOG_ERROR("Failed to open file: {}", filepath);
			return "";
		}
		std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		return source;
	}

	Vulkan_Shader::Vulkan_Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
	{
		auto renderContext = TAGE::Application::Get()->GetWindow()->GetRenderContext();
		_Device = reinterpret_cast<Device*>(renderContext->GetDevice());
	}

	Vulkan_Shader::~Vulkan_Shader()
	{
	}
	void Vulkan_Shader::Use() const
	{
	}
	void Vulkan_Shader::SetUniform(const char* name, bool value) const
	{
	}
	void Vulkan_Shader::SetUniform(const char* name, int value) const
	{
	}
	void Vulkan_Shader::SetUniform(const char* name, float value) const
	{
	}
	void Vulkan_Shader::SetUniform(const char* name, glm::vec2 value) const
	{
	}
	void Vulkan_Shader::SetUniform(const char* name, glm::vec3 value) const
	{
	}
	void Vulkan_Shader::SetUniform(const char* name, glm::vec4 value) const
	{
	}
	void Vulkan_Shader::SetUniform(const char* name, glm::mat3 value) const
	{
	}
	void Vulkan_Shader::SetUniform(const char* name, glm::mat4 value) const
	{
	}
	void Vulkan_Shader::DestroyProgram() const
	{
	}
	int Vulkan_Shader::GetUniformLocation(unsigned int program, const char* name) const
	{
		return 0;
	}
	void Vulkan_Shader::CheckCompileErrors(uint shader, const char* type) const
	{
	}
	void Vulkan_Shader::CompileShader(uint& shader, const char* code, const char* type)
	{
	}
	void Vulkan_Shader::CompileProgram(uint vertex, uint fragment, uint* geometry)
	{
	}
	void Vulkan_Shader::DeleteShader(uint shader)
	{
	}
}