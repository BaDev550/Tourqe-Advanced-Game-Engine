#include "tagepch.h"
#include "Shader.h"
#include "glm/gtc/type_ptr.hpp"

namespace TAGE::Renderer
{
	Shader::Shader(const char* vertexPath, const char* fragmentPath)
	{
		LoadShader(vertexPath, fragmentPath);
	}
	Shader::~Shader()
	{
		if (_ID != 0) {
			DeleteShader();
		}
		else {
			LOG_WARN("Shader program not initialized. No need to delete.");
		}
	}
	void Shader::Use() const
	{
		if (_ID == 0) {
			LOG_ERROR("Shader program not initialized. Cannot use shader.");
			return;
		}
		glUseProgram(_ID);
	}
	int GetUniformLocation(unsigned int program, const char* name) {
		int location = glGetUniformLocation(program, name);
		return location;
	}

	void Shader::SetUniform(const char* name, bool value) const { glUniform1i(GetUniformLocation(_ID, name), static_cast<int>(value)); }
	void Shader::SetUniform(const char* name, int value) const { glUniform1i(GetUniformLocation(_ID, name), value); }
	void Shader::SetUniform(const char* name, float value) const { glUniform1f(GetUniformLocation(_ID, name), value); }
	void Shader::SetUniform(const char* name, glm::vec2 value) const { glUniform2fv(GetUniformLocation(_ID, name), 1, glm::value_ptr(value)); }
	void Shader::SetUniform(const char* name, glm::vec3 value) const { glUniform3fv(GetUniformLocation(_ID, name), 1, glm::value_ptr(value)); }
	void Shader::SetUniform(const char* name, glm::vec4 value) const { glUniform4fv(GetUniformLocation(_ID, name), 1, glm::value_ptr(value)); }
	void Shader::SetUniform(const char* name, glm::mat3 value) const { glUniformMatrix3fv(GetUniformLocation(_ID, name), 1, GL_FALSE, glm::value_ptr(value)); }
	void Shader::SetUniform(const char* name, glm::mat4 value) const { glUniformMatrix4fv(GetUniformLocation(_ID, name), 1, GL_FALSE, glm::value_ptr(value)); }

	void Shader::CheckCompileErrors(uint shader, const char* type) const
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

	void Shader::LoadShader(const char* vertexPath, const char* fragmentPath)
	{
		std::ifstream vertexFile(vertexPath);
		std::ifstream fragmentFile(fragmentPath);
		if (!vertexFile.is_open() || !fragmentFile.is_open()) {
			LOG_CRITICAL("Failed to open shader files: {} or {}", vertexPath, fragmentPath);
			return;
		}
		std::unordered_set<std::string> included;

		std::stringstream vertexStream, fragmentStream;
		vertexStream << vertexFile.rdbuf();
		fragmentStream << fragmentFile.rdbuf();
		std::string vertexCode = ShaderPreprocessor::Preprocess(vertexPath);
		std::string fragmentCode = ShaderPreprocessor::Preprocess(fragmentPath);
		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();
		unsigned int vertex, fragment;

		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, nullptr);
		glCompileShader(vertex);
		CheckCompileErrors(vertex, "VERTEX");

		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, nullptr);
		glCompileShader(fragment);
		CheckCompileErrors(fragment, "FRAGMENT");

		_ID = glCreateProgram();
		glAttachShader(_ID, vertex);
		glAttachShader(_ID, fragment);
		glLinkProgram(_ID);
		CheckCompileErrors(_ID, "PROGRAM");

		glDeleteShader(vertex);
		glDeleteShader(fragment);
		vertexFile.close();
		fragmentFile.close();
	}
	void Shader::DeleteShader() const
	{
		if (_ID != 0) {
			glDeleteProgram(_ID);
			LOG_INFO("Shader program deleted successfully.");
		}
		else {
			LOG_WARN("Attempted to delete a shader program that was not initialized.");
		}
	}
}