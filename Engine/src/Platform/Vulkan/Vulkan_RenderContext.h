#pragma once

#include <vulkan/vulkan.h>
#include "TARE/Common/RenderContext.h"

struct GLFWwindow;
struct Device {
	VkPhysicalDevice physicalDevice;
	VkDevice logicalDevice;
};

namespace TARE {

	namespace Utilities {
		struct QueueFamilyIndices {
			int graphicsFamily = -1;
			bool isValid() { return graphicsFamily >= 0; }
		};
	}

	class Vulkan_RenderContext : public RenderContext
	{
	public:
		Vulkan_RenderContext(GLFWwindow* handle);
		~Vulkan_RenderContext();

		virtual void Init() override;
		virtual void SwapBuffers() override;
		virtual void* GetDevice() override {
			return &_MainDevice;
		}
		VkDevice Vulkan_RenderContext::GetVulkanDevice() const {
			return _MainDevice.logicalDevice;
		}
	private:
		Device _MainDevice;
		VkQueue graphicsQueue;

		GLFWwindow* _Window;

		VkInstance _Instance;
		void createVKInstance();
		void createVKLogicalDevice();
		bool checkInstanceExtensionSupport(std::vector<const char*>* checkExtensions);
		bool checkDeviceSuitable(VkPhysicalDevice device);

		void getPhysicalDevice();
		Utilities::QueueFamilyIndices getQueueFamilies(VkPhysicalDevice device);

		void PrintDeviceProperties(VkPhysicalDeviceProperties device);
	};
}