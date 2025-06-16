#pragma once
#include "TAGE/Common/TDefines.h"
#include "TAGE/Utilities/Logger.h"

#include "GLAD/glad.h"
#include "glm/glm.hpp"
#include "ShaderPreprocessor.h"

#include <string>
#include <unordered_set>

namespace TAGE::Renderer
{
	class Shader
	{
	public:
		Shader(const char* vertexPath, const char* fragmentPath);
		~Shader();
		void Use() const;
		void SetUniform(const char* name, bool value) const;
		void SetUniform(const char* name, int value) const;
		void SetUniform(const char* name, float value) const;
		void SetUniform(const char* name, glm::vec2 value) const;
		void SetUniform(const char* name, glm::vec3 value) const;
		void SetUniform(const char* name, glm::vec4 value) const;
		void SetUniform(const char* name, glm::mat3 value) const;
		void SetUniform(const char* name, glm::mat4 value) const;
		void DeleteShader() const;

		uint GetID() const { return _ID; }
	private:
		uint _ID;
		void CheckCompileErrors(uint shader, const char* type) const;
		void LoadShader(const char* vertexPath, const char* fragmentPath);
	};
}