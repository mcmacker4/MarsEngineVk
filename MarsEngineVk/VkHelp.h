#pragma once

#include <iostream>
#include <sstream>
#include <vector>

#ifdef _WIN32
#include <Windows.h>
#endif

#include <vulkan\vulkan.h>

namespace vkh {

	struct VulkanHandles {
		VkInstance *instance;
		VkDevice *device;
		uint32_t queueFamilyIndex;
	};

	void vulkanInit();
	void cleanUp();

	VkResult createInstance(const char* appName, uint32_t appVersion, VkInstance *pInstance, const void*);
	void destroyInstance(VkInstance instance);

	std::vector<VkPhysicalDevice> getPhysicalDevices(VkInstance *pInstance);
	int32_t getQueueFamilyIndex(VkPhysicalDevice physicalDevice, VkQueueFlags flags);
	VkDeviceQueueCreateInfo generateQueueCreateInfo(uint32_t queueCount, uint32_t queueFamilyIndex, float *priorities);

	VkResult createDevice(VkPhysicalDevice physicalDevice, VkDeviceQueueCreateInfo *queueCreateInfos, uint32_t queueCount, VkDevice* device);
	VkResult createDevice(VkPhysicalDevice physicalDevice, std::vector<VkDeviceQueueCreateInfo> queueCreateInfos, VkDevice* device);
	void destroyDevice(VkDevice device);

}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(
	VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT objectType,
	uint64_t object,
	size_t location,
	int32_t msgCode,
	const char* pLayerPrefix,
	const char* pMessage,
	void* pUserData
);
