#pragma once

#include "TARE/Shader/Shader.h"

namespace TARE::OpenGL {
	class OpenGL_Shader : public Shader
	{
	public:
		OpenGL_Shader(const std::string& shaderPath);
		~OpenGL_Shader();
		virtual void Use() const override;
		virtual void SetUniform(const std::string& name, bool value) const override;
		virtual void SetUniform(const std::string& name, int value) const override;
		virtual void SetUniform(const std::string& name, float value) const override;
		virtual void SetUniform(const std::string& name, glm::vec2 value) const override;
		virtual void SetUniform(const std::string& name, glm::vec3 value) const override;
		virtual void SetUniform(const std::string& name, glm::vec4 value) const override;
		virtual void SetUniform(const std::string& name, glm::mat3 value) const override;
		virtual void SetUniform(const std::string& name, glm::mat4 value) const override;

		virtual bool CreateUBO(const char* blockName, size_t size, unsigned int bindingPoint) override;
		virtual void UpdateUBO(unsigned int bindingPoint, const void* data, size_t size, size_t offset = 0) override;

		virtual bool CreateSSBO(unsigned int bindingPoint, size_t size) override;
		virtual void UpdateSSBO(unsigned int bindingPoint, const void* data, size_t size) override;
		virtual bool Reload() override;
	private:
		bool LinkUBOToBindingPoint(const char* blockName, unsigned int bindingPoint);
		void DeleteShader(uint shader);

		uint _ID;
		std::unordered_map<uint, uint> _ubos;
		std::unordered_map<uint, uint> _ssbos; // to-do find better way to handle uniform/shader storage buffer objects
	};
}
