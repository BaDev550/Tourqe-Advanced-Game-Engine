#pragma once
#include "TARE/Shader/Shader.h"
#include <vulkan/vulkan.h>

struct Device;
namespace TARE {
    class Vulkan_Shader : public Shader
    {
    public:
        Vulkan_Shader(const std::string& vertPath, const std::string& fragPath);
        ~Vulkan_Shader();

        virtual void Use() const override;
        virtual void SetUniform(const char* name, bool value) const override {}
        virtual void SetUniform(const char* name, int value) const override {}
        virtual void SetUniform(const char* name, float value) const override {}
        virtual void SetUniform(const char* name, glm::vec2 value) const override {}
        virtual void SetUniform(const char* name, glm::vec3 value) const override {}
        virtual void SetUniform(const char* name, glm::vec4 value) const override {}
        virtual void SetUniform(const char* name, glm::mat3 value) const override {}
        virtual void SetUniform(const char* name, glm::mat4 value) const override {}

        virtual void DestroyProgram() const override;

        virtual int GetUniformLocation(unsigned int program, const char* name) const override { return -1; }

        virtual bool CreateUBO(const char* blockName, size_t size, unsigned int bindingPoint) override;
        virtual void UpdateUBO(unsigned int bindingPoint, const void* data, size_t size) override;

        virtual bool CreateSSBO(unsigned int bindingPoint, size_t size) override;
        virtual void UpdateSSBO(unsigned int bindingPoint, const void* data, size_t size) override;
    protected:
        virtual bool LinkUBOToBindingPoint(const char* blockName, unsigned int bindingPoint) override { return true; }
    private:
        VkDevice _device = VK_NULL_HANDLE;
        VkShaderModule _vertShaderModule = VK_NULL_HANDLE;
        VkShaderModule _fragShaderModule = VK_NULL_HANDLE;

        struct BufferInfo {
            VkBuffer buffer = VK_NULL_HANDLE;
            VkDeviceMemory memory = VK_NULL_HANDLE;
            size_t size = 0;
        };

        std::unordered_map<unsigned int, BufferInfo> _uboBuffers;
        std::unordered_map<unsigned int, BufferInfo> _ssboBuffers;

        bool CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
        std::vector<char> ReadFile(const std::string& filename);
        virtual void CheckCompileErrors(uint shader, const char* type) const override {};
        virtual void CompileShader(uint& shader, const char* code, const char* type) override {};
        virtual void CompileProgram(uint vertex, uint fragment, uint* geometry) override {};
        virtual void DeleteShader(uint shader) override {};
    };
}