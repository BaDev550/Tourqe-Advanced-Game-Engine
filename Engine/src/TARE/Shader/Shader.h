#pragma once
#include "TAGE/Common/TDefines.h"
#include "TAGE/Utilities/Logger.h"
#include "glm/glm.hpp"

namespace TARE
{
	class Shader
	{
	public:
		virtual ~Shader();
		virtual void Use() const = 0;
		virtual void SetUniform(const char* name, bool value) const = 0;
		virtual void SetUniform(const char* name, int value) const = 0;
		virtual void SetUniform(const char* name, float value) const = 0;
		virtual void SetUniform(const char* name, glm::vec2 value) const = 0;
		virtual void SetUniform(const char* name, glm::vec3 value) const = 0;
		virtual void SetUniform(const char* name, glm::vec4 value) const = 0;
		virtual void SetUniform(const char* name, glm::mat3 value) const = 0;
		virtual void SetUniform(const char* name, glm::mat4 value) const = 0;
		virtual void DestroyProgram() const {};
		virtual int GetUniformLocation(unsigned int program, const char* name) const { return 0; }
		virtual bool CreateUBO(const char* blockName, size_t size, unsigned int bindingPoint) { return 0; }
		virtual void UpdateUBO(unsigned int bindingPoint, const void* data, size_t size, size_t offset = 0) {}
		virtual bool CreateSSBO(unsigned int bindingPoint, size_t size) { return false; }
		virtual void UpdateSSBO(unsigned int bindingPoint, const void* data, size_t size) {}
		bool Reload();
		std::string& GetFragmentSource() { return _FragmentSource; }

		static TAGE::MEM::Ref<Shader> Create(const char* vertexPath, const char* fragmentPath, const char* geometryPath = "0");
	protected:
		virtual bool LinkUBOToBindingPoint(const char* blockName, unsigned int bindingPoint) { return 0; }

		virtual void CheckCompileErrors(uint shader, const char* type) const = 0;
		virtual void CompileShader(uint& shader, const char* code, const char* type) = 0;
		virtual void CompileProgram(uint vertex, uint fragment, uint* geometry) = 0;
		virtual void DeleteShader(uint shader) = 0;

		void LoadShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath);
		std::string _VertexPath, _FragmentPath, _GeometryPath, _FragmentSource;
	};
}