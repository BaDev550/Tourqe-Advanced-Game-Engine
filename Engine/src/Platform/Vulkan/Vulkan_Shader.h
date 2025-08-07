#pragma once
#include "TARE/Shader/Shader.h"
#include <vulkan/vulkan.h>

struct Device;
namespace TARE {
	class Vulkan_Shader : public Shader
	{
	public:
		Vulkan_Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath);
		~Vulkan_Shader();

		virtual void Use() const override;
		virtual void SetUniform(const char* name, bool value) const override;
		virtual void SetUniform(const char* name, int value) const override;
		virtual void SetUniform(const char* name, float value) const override;
		virtual void SetUniform(const char* name, glm::vec2 value) const override;
		virtual void SetUniform(const char* name, glm::vec3 value) const override;
		virtual void SetUniform(const char* name, glm::vec4 value) const override;
		virtual void SetUniform(const char* name, glm::mat3 value) const override;
		virtual void SetUniform(const char* name, glm::mat4 value) const override;
		virtual void DestroyProgram() const override;
	protected:
		virtual int GetUniformLocation(unsigned int program, const char* name) const override;
		virtual void CheckCompileErrors(uint shader, const char* type) const override;
		virtual void CompileShader(uint& shader, const char* code, const char* type) override;
		virtual void CompileProgram(uint vertex, uint fragment, uint* geometry) override;
		virtual void DeleteShader(uint shader) override;

	private:
		uint _ID;

		struct PushConstantData {
			glm::vec3 u_CameraPos;
			float u_Exposure;

			glm::mat4 u_View;
			glm::mat4 u_Projection;
			glm::mat4 u_InverseView;
			glm::mat4 u_InverseProjection;
			glm::mat4 u_PrevViewProj;
			glm::mat4 u_CurrViewProj;
		} _pushConstants{};

		Device* _Device;
		VkShaderModule _VertexShaderModule = VK_NULL_HANDLE;
		VkShaderModule _FragmentShaderModule = VK_NULL_HANDLE;
		VkShaderModule _GeometryShaderModule = VK_NULL_HANDLE;

		std::vector<char> ReadFileBinary(const std::string& filepath) const;
		VkShaderModule CreateShaderModule(const std::vector<char>& code) const;
	};
}