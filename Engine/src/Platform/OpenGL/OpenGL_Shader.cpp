#include "tagepch.h"
#include "OpenGL_Shader.h"
#include "glm/gtc/type_ptr.hpp"

#include <GLAD/glad.h>

#include "OpenGL_ShaderCompiler.h"

namespace TARE::OpenGL {
	OpenGL_Shader::OpenGL_Shader(const std::string& shaderPath)
	{
		_ShaderPath = shaderPath;
		OpenGL_ShaderCompiler compiler(_ShaderPath);
		compiler.Compile();
		_ID = compiler.GetProgram();
	}

	OpenGL_Shader::~OpenGL_Shader()
	{
		glDeleteProgram(_ID);
	}

	void OpenGL_Shader::Use() const
	{
		glUseProgram(_ID);
	}

	int GetUniformLocation(unsigned int program, std::string name)
	{
		int location = glGetUniformLocation(program, name.c_str());
		return location;
	}

	void OpenGL_Shader::SetUniform(const std::string& name, bool value) const { glUniform1i(GetUniformLocation(_ID, name), static_cast<int>(value)); }
	void OpenGL_Shader::SetUniform(const std::string& name, int value) const { glUniform1i(GetUniformLocation(_ID, name), value); }
	void OpenGL_Shader::SetUniform(const std::string& name, float value) const { glUniform1f(GetUniformLocation(_ID, name), value); }
	void OpenGL_Shader::SetUniform(const std::string& name, glm::vec2 value) const { glUniform2fv(GetUniformLocation(_ID, name), 1, glm::value_ptr(value)); }
	void OpenGL_Shader::SetUniform(const std::string& name, glm::vec3 value) const { glUniform3fv(GetUniformLocation(_ID, name), 1, glm::value_ptr(value)); }
	void OpenGL_Shader::SetUniform(const std::string& name, glm::vec4 value) const { glUniform4fv(GetUniformLocation(_ID, name), 1, glm::value_ptr(value)); }
	void OpenGL_Shader::SetUniform(const std::string& name, glm::mat3 value) const { glUniformMatrix3fv(GetUniformLocation(_ID, name), 1, GL_FALSE, glm::value_ptr(value)); }
	void OpenGL_Shader::SetUniform(const std::string& name, glm::mat4 value) const { glUniformMatrix4fv(GetUniformLocation(_ID, name), 1, GL_FALSE, glm::value_ptr(value)); }

	bool OpenGL_Shader::LinkUBOToBindingPoint(const char* blockName, unsigned int bindingPoint)
	{
		int blockIndex = glGetUniformBlockIndex(_ID, blockName);
		if (blockIndex == GL_INVALID_INDEX) {
			return false;
		}
		glUniformBlockBinding(_ID, blockIndex, bindingPoint);
		return true;
	}

	void OpenGL_Shader::DeleteShader(uint shader)
	{
		if (shader != 0)
			glDeleteShader(shader);
	}

	bool OpenGL_Shader::CreateUBO(const char* blockName, size_t size, unsigned int bindingPoint)
	{
		GLuint ubo;
		glGenBuffers(1, &ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, ubo);

		if (!LinkUBOToBindingPoint(blockName, bindingPoint)) {
			glDeleteBuffers(1, &ubo);
			LOG_CRITICAL("Failed to link UBO '{}' to binding point {}. Ensure the block name is correct and the shader is compiled.", blockName, bindingPoint);
			return false;
		}

		_ubos[bindingPoint] = ubo;
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		LOG_INFO("UBO '{}' created and linked to binding point {} successfully.", blockName, bindingPoint);
		return true;
	}

	void OpenGL_Shader::UpdateUBO(unsigned int bindingPoint, const void* data, size_t size, size_t offset)
	{
		auto it = _ubos.find(bindingPoint);
		if (it == _ubos.end())
			return;

		glBindBuffer(GL_UNIFORM_BUFFER, it->second);
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	bool OpenGL_Shader::CreateSSBO(unsigned int bindingPoint, size_t size)
	{
		GLuint ssbo;
		glGenBuffers(1, &ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		LOG_INFO("SSBO created and linked to binding point {} successfully.", bindingPoint);

		_ssbos[bindingPoint] = ssbo;
		return true;
	}

	void OpenGL_Shader::UpdateSSBO(unsigned int bindingPoint, const void* data, size_t size)
	{
		auto it = _ssbos.find(bindingPoint);
		if (it == _ssbos.end()) return;
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, it->second);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, size, data);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	bool OpenGL_Shader::Reload()
	{
		return false;
	}
}