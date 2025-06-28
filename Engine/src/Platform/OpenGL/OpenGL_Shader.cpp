#include "tagepch.h"
#include "OpenGL_Shader.h"
#include "glm/gtc/type_ptr.hpp"
#include <GLAD/glad.h>

namespace TARE {
	OpenGL_Shader::OpenGL_Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
	{
		LoadShader(vertexPath, fragmentPath, geometryPath);
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