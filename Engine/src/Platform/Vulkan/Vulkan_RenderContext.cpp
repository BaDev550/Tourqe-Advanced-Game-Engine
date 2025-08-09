#include "tagepch.h"
#include "Vulkan_RenderContext.h"
#include <GLFW/glfw3.h>

TARE::Vulkan_RenderContext::Vulkan_RenderContext(GLFWwindow* handle)
	: _Window(handle)
{
}

TARE::Vulkan_RenderContext::~Vulkan_RenderContext()
{
	vkDestroyDevice(_MainDevice.logicalDevice, nullptr);
	vkDestroyInstance(_Instance, nullptr);
}

void TARE::Vulkan_RenderContext::Init()
{
	try {
		createVKInstance();
		getPhysicalDevice();
		createVKLogicalDevice();
	}
	catch (const std::runtime_error& e) {
		LOG_ERROR("ERROR: {}", e.what());
	}
}

void TARE::Vulkan_RenderContext::SwapBuffers()
{
}

void TARE::Vulkan_RenderContext::createVKInstance()
{
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = glfwGetWindowTitle(_Window);
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "NONE";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_4;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	uint glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> instanceExtensions = {};
	for (size_t i = 0; i < glfwExtensionCount; i++)
		instanceExtensions.push_back(glfwExtensions[i]);

	if (!checkInstanceExtensionSupport(&instanceExtensions))
		throw std::runtime_error("VKInstance does not support required extencions");

	createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
	createInfo.ppEnabledExtensionNames = instanceExtensions.data();
	createInfo.enabledLayerCount = 0;
	createInfo.ppEnabledLayerNames = nullptr;

	VkResult result = vkCreateInstance(&createInfo, nullptr, &_Instance);
	if (result != VK_SUCCESS)
		throw std::runtime_error("Failed to create a Vulkan Instance");
}

void TARE::Vulkan_RenderContext::createVKLogicalDevice()
{
	Utilities::QueueFamilyIndices indices = getQueueFamilies(_MainDevice.physicalDevice);
	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;
	queueCreateInfo.queueCount = 1;
	float priority = 1.0f;
	queueCreateInfo.pQueuePriorities = &priority;

	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
	deviceCreateInfo.enabledExtensionCount = 0;
	deviceCreateInfo.ppEnabledExtensionNames = nullptr;

	VkPhysicalDeviceFeatures deviceFeatures = {};
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
	VkResult result = vkCreateDevice(_MainDevice.physicalDevice, &deviceCreateInfo, nullptr, &_MainDevice.logicalDevice);
	if (result != VK_SUCCESS)
		throw std::runtime_error("Failed to create logical device!");

	vkGetDeviceQueue(_MainDevice.logicalDevice, indices.graphicsFamily, 0, &graphicsQueue);
}

bool TARE::Vulkan_RenderContext::checkInstanceExtensionSupport(std::vector<const char*>* checkExtensions)
{
	uint extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	for (const auto& checkExtension : *checkExtensions) {
		bool hasExtension = false;
		for (const auto& extension : extensions) {
			if (strcmp(checkExtension, extension.extensionName) == 0) {
				hasExtension = true;
				break;
			}
		}
		if (!hasExtension)
			return false;
	}
	return true;
}

bool TARE::Vulkan_RenderContext::checkDeviceSuitable(VkPhysicalDevice device)
{
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	PrintDeviceProperties(deviceProperties);
	//VkPhysicalDeviceFeatures deviceFeatures;
	//vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	Utilities::QueueFamilyIndices indices = getQueueFamilies(device);
	return indices.isValid();
}

void TARE::Vulkan_RenderContext::getPhysicalDevice()
{
	uint deviceCount = 0;
	vkEnumeratePhysicalDevices(_Instance, &deviceCount, nullptr);
	if (deviceCount == 0)
		throw std::runtime_error("Can't find GPU support vulkan!");

	std::vector<VkPhysicalDevice> deviceList(deviceCount);
	vkEnumeratePhysicalDevices(_Instance, &deviceCount, deviceList.data());

	for (const auto& device : deviceList) {
		if (checkDeviceSuitable(device)) {
			_MainDevice.physicalDevice = device;
			break;
		}
	}
}

TARE::Utilities::QueueFamilyIndices TARE::Vulkan_RenderContext::getQueueFamilies(VkPhysicalDevice device)
{
	Utilities::QueueFamilyIndices indices;
	uint queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilyList(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyList.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilyList) {
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			indices.graphicsFamily = i;

		if (indices.isValid())
			break;

		i++;
	}
	return indices;
}

void TARE::Vulkan_RenderContext::PrintDeviceProperties(VkPhysicalDeviceProperties device)
{
	LOG_INFO("API Version {}", device.apiVersion);
	LOG_INFO("GPU Name: {}", device.deviceName);
}
