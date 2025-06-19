#include "tagepch.h"
#include "DirectX11_Shader.h"
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <glm/gtc/type_ptr.hpp>

namespace TARE {
	DirectX11_Shader::DirectX11_Shader(const char* vertexPath, const char* fragmentPath)
	{
	}
	DirectX11_Shader::~DirectX11_Shader()
	{
	}
	void DirectX11_Shader::Use() const
	{
	}
	void DirectX11_Shader::SetUniform(const char* name, bool value) const
	{
	}
	void DirectX11_Shader::SetUniform(const char* name, int value) const
	{
	}
	void DirectX11_Shader::SetUniform(const char* name, float value) const
	{
	}
	void DirectX11_Shader::SetUniform(const char* name, glm::vec2 value) const
	{
	}
	void DirectX11_Shader::SetUniform(const char* name, glm::vec3 value) const
	{
	}
	void DirectX11_Shader::SetUniform(const char* name, glm::vec4 value) const
	{
	}
	void DirectX11_Shader::SetUniform(const char* name, glm::mat3 value) const
	{
	}
	void DirectX11_Shader::SetUniform(const char* name, glm::mat4 value) const
	{
	}
	void DirectX11_Shader::DestroyProgram() const
	{
	}
	int DirectX11_Shader::GetUniformLocation(unsigned int program, const char* name) const
	{
		return 0;
	}
	void DirectX11_Shader::CheckCompileErrors(uint shader, const char* type) const
	{
	}
	void DirectX11_Shader::CompileShader(uint& shader, const char* code, const char* type)
	{
	}
	void DirectX11_Shader::CompileProgram(uint vertex, uint fragment)
	{
	}
	void DirectX11_Shader::DeleteShader(uint shader)
	{
	}
}