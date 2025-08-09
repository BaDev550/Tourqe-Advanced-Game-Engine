#include "tagepch.h"
#include "Vulkan_Shader.h"
#include "Vulkan_ShaderCompiler.h"
#include "Vulkan_RenderContext.h"
#include "TAGE/Application/Application.h"

namespace TARE {
    Vulkan_Shader::Vulkan_Shader(const std::string& vertPath, const std::string& fragPath)
    {
		_device = static_cast<Vulkan_RenderContext*>(TAGE::Application::Get()->GetWindow()->GetRenderContext()->GetDevice())->GetVulkanDevice();
        auto vertCode = ReadFile(vertPath);
        auto fragCode = ReadFile(fragPath);

        if (!CreateShaderModule(vertCode, &_vertShaderModule))
            throw std::runtime_error("Failed to create vertex shader module");

        if (!CreateShaderModule(fragCode, &_fragShaderModule))
            throw std::runtime_error("Failed to create fragment shader module");
    }

    Vulkan_Shader::~Vulkan_Shader()
    {
        for (auto& [_, buffer] : _uboBuffers)
        {
            if (buffer.buffer != VK_NULL_HANDLE) vkDestroyBuffer(_device, buffer.buffer, nullptr);
            if (buffer.memory != VK_NULL_HANDLE) vkFreeMemory(_device, buffer.memory, nullptr);
        }
        for (auto& [_, buffer] : _ssboBuffers)
        {
            if (buffer.buffer != VK_NULL_HANDLE) vkDestroyBuffer(_device, buffer.buffer, nullptr);
            if (buffer.memory != VK_NULL_HANDLE) vkFreeMemory(_device, buffer.memory, nullptr);
        }

        if (_vertShaderModule != VK_NULL_HANDLE)
            vkDestroyShaderModule(_device, _vertShaderModule, nullptr);

        if (_fragShaderModule != VK_NULL_HANDLE)
            vkDestroyShaderModule(_device, _fragShaderModule, nullptr);
    }

    void Vulkan_Shader::Use() const
    {
    }

    bool Vulkan_Shader::CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule)
    {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        if (vkCreateShaderModule(_device, &createInfo, nullptr, shaderModule) != VK_SUCCESS)
        {
            LOG_CRITICAL("Failed to create shader module!");
            return false;
        }
        return true;
    }

    std::vector<char> Vulkan_Shader::ReadFile(const std::string& filename)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);
        if (!file.is_open())
            throw std::runtime_error("failed to open file: " + filename);

        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();

        return buffer;
    }

    bool Vulkan_Shader::CreateUBO(const char* blockName, size_t size, unsigned int bindingPoint)
    {
        BufferInfo bufferInfo{};
        bufferInfo.size = size;
        _uboBuffers[bindingPoint] = bufferInfo;
        return true;
    }

    void Vulkan_Shader::UpdateUBO(unsigned int bindingPoint, const void* data, size_t size)
    {
        auto it = _uboBuffers.find(bindingPoint);
        if (it == _uboBuffers.end())
            return;

        BufferInfo& buf = it->second;

        void* mapped;
        vkMapMemory(_device, buf.memory, 0, size, 0, &mapped);
        memcpy(mapped, data, size);
        vkUnmapMemory(_device, buf.memory);
    }

    bool Vulkan_Shader::CreateSSBO(unsigned int bindingPoint, size_t size)
    {
        BufferInfo bufferInfo{};
        bufferInfo.size = size;
        _ssboBuffers[bindingPoint] = bufferInfo;
        return true;
    }

    void Vulkan_Shader::UpdateSSBO(unsigned int bindingPoint, const void* data, size_t size)
    {
        auto it = _ssboBuffers.find(bindingPoint);
        if (it == _ssboBuffers.end())
            return;

        BufferInfo& buf = it->second;

        void* mapped;
        vkMapMemory(_device, buf.memory, 0, size, 0, &mapped);
        memcpy(mapped, data, size);
        vkUnmapMemory(_device, buf.memory);
    }

    void Vulkan_Shader::DestroyProgram() const
    {
    }
}