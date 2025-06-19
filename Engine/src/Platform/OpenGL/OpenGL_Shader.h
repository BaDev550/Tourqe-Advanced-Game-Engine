#pragma once
#include "TARE/Shader/Shader.h"

namespace TARE {
	class OpenGL_Shader : public Shader
	{
	public:
		OpenGL_Shader(const char* vertexPath, const char* fragmentPath);

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
		virtual void CompileProgram(uint vertex, uint fragment) override;
		virtual void DeleteShader(uint shader) override;

	private:
		uint _ID;
	};
}