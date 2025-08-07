#include "tagepch.h"
#include <TAGE/Application/Application.h>
#include "Vulkan_Shader.h"
#include "Vulkan_ShaderCompiler.h"
#include "Vulkan_RenderContext.h"

namespace TARE {
	Vulkan_Shader::Vulkan_Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
	{
		auto renderContext = TAGE::Application::Get()->GetWindow()->GetRenderContext();
		_Device = reinterpret_cast<Device*>(renderContext->GetDevice());

		auto vertCode = ReadFileBinary(vertexPath);
		auto fragCode = ReadFileBinary(fragmentPath);

		_VertexShaderModule = CreateShaderModule(vertCode);
		_FragmentShaderModule = CreateShaderModule(fragCode);

		if (geometryPath) {
			auto geomCode = ReadFileBinary(geometryPath);
			_GeometryShaderModule = CreateShaderModule(geomCode);
		}
	}

	Vulkan_Shader::~Vulkan_Shader()
	{
		if (_VertexShaderModule != VK_NULL_HANDLE)
			vkDestroyShaderModule(_Device->logicalDevice, _VertexShaderModule, nullptr);
		if (_FragmentShaderModule != VK_NULL_HANDLE)
			vkDestroyShaderModule(_Device->logicalDevice, _FragmentShaderModule, nullptr);
		if (_GeometryShaderModule != VK_NULL_HANDLE)
			vkDestroyShaderModule(_Device->logicalDevice, _GeometryShaderModule, nullptr);
	}

	void Vulkan_Shader::Use() const
	{
	}

	void Vulkan_Shader::SetUniform(const char* name, bool value) const {}
	void Vulkan_Shader::SetUniform(const char* name, int value) const {}
	void Vulkan_Shader::SetUniform(const char* name, float value) const {}
	void Vulkan_Shader::SetUniform(const char* name, glm::vec2 value) const {}
	void Vulkan_Shader::SetUniform(const char* name, glm::vec3 value) const {}
	void Vulkan_Shader::SetUniform(const char* name, glm::vec4 value) const {}
	void Vulkan_Shader::SetUniform(const char* name, glm::mat3 value) const {}
	void Vulkan_Shader::SetUniform(const char* name, glm::mat4 value) const {}

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

	std::vector<char> Vulkan_Shader::ReadFileBinary(const std::string& filepath) const
	{
		std::ifstream file(filepath, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			LOG_ERROR("Failed to open shader file: {}", filepath);
			throw std::runtime_error("Failed to open shader file: " + filepath);
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();

		return buffer;
	}

	VkShaderModule Vulkan_Shader::CreateShaderModule(const std::vector<char>& code) const
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(_Device->logicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create shader module.");
		}

		return shaderModule;
	}
}