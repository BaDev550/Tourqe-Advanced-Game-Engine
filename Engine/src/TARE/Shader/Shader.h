#pragma once
#include "TAGE/Utilities/Memory.h"
#include "glm/glm.hpp"

namespace TARE {
	class Shader
	{
	public:
		virtual ~Shader() = default;
		virtual void Use() const = 0;
		virtual void SetUniform(const std::string& name, bool value) const = 0;
		virtual void SetUniform(const std::string& name, int value) const = 0;
		virtual void SetUniform(const std::string& name, float value) const = 0;
		virtual void SetUniform(const std::string& name, glm::vec2 value) const = 0;
		virtual void SetUniform(const std::string& name, glm::vec3 value) const = 0;
		virtual void SetUniform(const std::string& name, glm::vec4 value) const = 0;
		virtual void SetUniform(const std::string& name, glm::mat3 value) const = 0;
		virtual void SetUniform(const std::string& name, glm::mat4 value) const = 0;

		virtual bool CreateUBO(const char* blockName, size_t size, unsigned int bindingPoint) { return 0; }
		virtual void UpdateUBO(unsigned int bindingPoint, const void* data, size_t size, size_t offset = 0) {}

		virtual bool CreateSSBO(unsigned int bindingPoint, size_t size) { return false; }
		virtual void UpdateSSBO(unsigned int bindingPoint, const void* data, size_t size) {}
		virtual bool Reload() = 0;

		static TAGE::MEM::Ref<Shader> Create(const std::string& shaderPath);
	protected:
		std::string _ShaderPath;
	};
}
