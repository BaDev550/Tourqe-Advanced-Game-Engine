#pragma once
#include "TARE/Shader/Shader.h"

namespace TARE {
	class OpenGL_Shader : public Shader
	{
	public:
		OpenGL_Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath);
		~OpenGL_Shader();

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
		virtual int GetUniformLocation(unsigned int program, const char* name) const override;
		virtual bool CreateUBO(const char* blockName, size_t size, unsigned int bindingPoint) override;
		virtual void UpdateUBO(unsigned int bindingPoint, const void* data, size_t size) override;
		virtual bool CreateSSBO(unsigned int bindingPoint, size_t size) override;
		virtual void UpdateSSBO(unsigned int bindingPoint, const void* data, size_t size) override;
	protected:
		virtual bool LinkUBOToBindingPoint(const char* blockName, unsigned int bindingPoint);

		virtual void CheckCompileErrors(uint shader, const char* type) const override;
		virtual void CompileShader(uint& shader, const char* code, const char* type) override;
		virtual void CompileProgram(uint vertex, uint fragment, uint* geometry) override;
		virtual void DeleteShader(uint shader) override;
	private:
		uint _ID;
		std::unordered_map<unsigned int, unsigned int> _ubos;
		std::unordered_map<unsigned int, unsigned int> _ssbos;
	};
}