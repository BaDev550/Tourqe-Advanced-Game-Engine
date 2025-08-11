#include "tagepch.h"
#include "OpenGL_Shader.h"
#include "glm/gtc/type_ptr.hpp"
#include <GLAD/glad.h>

namespace TARE {
	OpenGL_Shader::OpenGL_Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
	{
		LoadShader(vertexPath, fragmentPath, geometryPath);
	}
	OpenGL_Shader::~OpenGL_Shader()
	{
		glDeleteProgram(_ID);
	}
	void OpenGL_Shader::Use() const
	{
		glUseProgram(_ID);
	}

	int OpenGL_Shader::GetUniformLocation(unsigned int program, const char* name) const
	{
		int location = glGetUniformLocation(program, name);
		return location;
	}

	void OpenGL_Shader::SetUniform(const char* name, bool value) const { glUniform1i(GetUniformLocation(_ID, name), static_cast<int>(value)); }
	void OpenGL_Shader::SetUniform(const char* name, int value) const { glUniform1i(GetUniformLocation(_ID, name), value); }
	void OpenGL_Shader::SetUniform(const char* name, float value) const { glUniform1f(GetUniformLocation(_ID, name), value); }
	void OpenGL_Shader::SetUniform(const char* name, glm::vec2 value) const { glUniform2fv(GetUniformLocation(_ID, name), 1, glm::value_ptr(value)); }
	void OpenGL_Shader::SetUniform(const char* name, glm::vec3 value) const { glUniform3fv(GetUniformLocation(_ID, name), 1, glm::value_ptr(value)); }
	void OpenGL_Shader::SetUniform(const char* name, glm::vec4 value) const { glUniform4fv(GetUniformLocation(_ID, name), 1, glm::value_ptr(value)); }
	void OpenGL_Shader::SetUniform(const char* name, glm::mat3 value) const { glUniformMatrix3fv(GetUniformLocation(_ID, name), 1, GL_FALSE, glm::value_ptr(value)); }
	void OpenGL_Shader::SetUniform(const char* name, glm::mat4 value) const { glUniformMatrix4fv(GetUniformLocation(_ID, name), 1, GL_FALSE, glm::value_ptr(value)); }

	void OpenGL_Shader::DestroyProgram() const
	{
		glDeleteProgram(_ID);
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

	bool OpenGL_Shader::LinkUBOToBindingPoint(const char* blockName, unsigned int bindingPoint)
	{
		int blockIndex = glGetUniformBlockIndex(_ID, blockName);
		if (blockIndex == GL_INVALID_INDEX) {
			return false;
		}
		glUniformBlockBinding(_ID, blockIndex, bindingPoint);
		return true;
	}

	void OpenGL_Shader::CompileShader(uint& shader, const char* code, const char* type)
	{
		if (std::strcmp(type, "VERTEX") == 0)
			shader = glCreateShader(GL_VERTEX_SHADER);
		else if (std::strcmp(type, "FRAGMENT") == 0)
			shader = glCreateShader(GL_FRAGMENT_SHADER);
		else if (std::strcmp(type, "GEOMETRY") == 0)
			shader = glCreateShader(GL_GEOMETRY_SHADER);
		else {
			LOG_ERROR("Unknown shader type: {}", type);
			return;
		}

		//LOG_INFO("SHADER CODE:\n{}", code);
		glShaderSource(shader, 1, &code, nullptr);
		glCompileShader(shader);
		CheckCompileErrors(shader, type);
	}

	void OpenGL_Shader::CompileProgram(uint vertex, uint fragment, uint* geometry)
	{
		_ID = glCreateProgram();
		glAttachShader(_ID, vertex);
		glAttachShader(_ID, fragment);
		if (geometry)
			glAttachShader(_ID, *geometry);

		glLinkProgram(_ID);
		CheckCompileErrors(_ID, "PROGRAM");
	}

	void OpenGL_Shader::DeleteShader(uint shader)
	{
		if (shader != 0)
			glDeleteShader(shader);
	}

	void OpenGL_Shader::CheckCompileErrors(uint shader, const char* type) const
	{
		int success;
		char infoLog[1024];
		if (std::string(type) != "PROGRAM") {
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
				LOG_ERROR("Shader compilation error in {}: {}\n", type, infoLog);
			}
			else {
			}
		}
		else {
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
				LOG_ERROR("Program linking error: {}\n", infoLog);
			}
			else {
			}
		}
	}
}