#pragma once
#include "TARE/Shader/Shader.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <unordered_map>

namespace TARE {
	class DirectX11_Shader : public Shader
	{
	public:
		DirectX11_Shader(const char* vertexPath, const char* fragmentPath);
		~DirectX11_Shader();

		void Use() const override;

		void SetUniform(const char* name, bool value) const override;
		void SetUniform(const char* name, int value) const override;
		void SetUniform(const char* name, float value) const override;
		void SetUniform(const char* name, glm::vec2 value) const override;
		void SetUniform(const char* name, glm::vec3 value) const override;
		void SetUniform(const char* name, glm::vec4 value) const override;
		void SetUniform(const char* name, glm::mat3 value) const override;
		void SetUniform(const char* name, glm::mat4 value) const override;

		void DestroyProgram() const override;
	protected:
		int GetUniformLocation(unsigned int program, const char* name) const override;
		void CheckCompileErrors(uint shader, const char* type) const override;
		void CompileShader(uint& shader, const char* code, const char* type) override;
		void CompileProgram(uint vertex, uint fragment) override;
		void DeleteShader(uint shader) override;
	};
}