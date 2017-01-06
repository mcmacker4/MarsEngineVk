#include "VkHelp.h"

namespace vkh {

	std::vector<const char*> instanceExtensions;
	std::vector<const char*> instanceLayers;
	VulkanHandles handles{};

#ifdef _DEBUG
	VkDebugReportCallbackEXT debugReportCallback;
	VkDebugReportCallbackCreateInfoEXT debugCallbackCreateInfo{};

	PFN_vkCreateDebugReportCallbackEXT fvkCreateDebugReportCallback;
	PFN_vkDestroyDebugReportCallbackEXT fvkDestroyDebugReportCallback;
#endif

	void vulkanInit() {

		//Debug init
#ifdef _DEBUG
		instanceLayers.push_back("VK_LAYER_LUNARG_standard_validation");
		instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

		debugCallbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		debugCallbackCreateInfo.pfnCallback = VulkanDebugCallback;
		debugCallbackCreateInfo.flags = VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
			VK_DEBUG_REPORT_WARNING_BIT_EXT |
			VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
			VK_DEBUG_REPORT_ERROR_BIT_EXT |
			VK_DEBUG_REPORT_DEBUG_BIT_EXT;
#endif

		//Create Vulkan Instance
		handles.instance = new VkInstance;
#ifdef _DEBUG
		VkResult err = createInstance("Hello Vulkan", VK_MAKE_VERSION(0, 1, 0), handles.instance, &debugCallbackCreateInfo);
#else
		VkResult err = createInstance("Hello Vulkan", VK_MAKE_VERSION(0, 1, 0), handles.instance, nullptr);
#endif
		if (err != VK_SUCCESS)
			throw "Could not create Vulkan Instance";

		std::cout << "Instance created." << std::endl;

		//Finalize Debug setup.
#ifdef _DEBUG
		fvkCreateDebugReportCallback = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(*handles.instance, "vkCreateDebugReportCallbackEXT");
		fvkDestroyDebugReportCallback = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(*handles.instance, "vkDestroyDebugReportCallbackEXT");
		fvkCreateDebugReportCallback(*handles.instance, &debugCallbackCreateInfo, nullptr, &debugReportCallback);
#endif

		//Get Physical device
		std::vector<VkPhysicalDevice> physicalDevices = getPhysicalDevices(handles.instance);
		std::cout << "Found " << physicalDevices.size() << " physical devices." << std::endl;
		VkPhysicalDevice physicalDevice = physicalDevices[0];
		std::cout << "Assuming physical device at index 0 to be correct." << std::endl;

		handles.queueFamilyIndex = getQueueFamilyIndex(physicalDevice, VK_QUEUE_GRAPHICS_BIT);
		float queuePriorities[]{ 1.0 };

		//Create Device
		handles.device = new VkDevice;
		VkDeviceQueueCreateInfo queueCreateInfo = generateQueueCreateInfo(1, handles.queueFamilyIndex, queuePriorities);
		err = createDevice(physicalDevice, &queueCreateInfo, 1, handles.device);
		if (err != VK_SUCCESS)
			throw "Could not create Vulkan Device";

		std::cout << "Device created." << std::endl;
	}

	void cleanUp() {
		destroyDevice(*handles.device);
		std::cout << "Device destroyed." << std::endl;

#ifdef _DEBUG
		fvkDestroyDebugReportCallback(*handles.instance, debugReportCallback, nullptr);
#endif

		destroyInstance(*handles.instance);
		std::cout << "Instance destroyed." << std::endl;
	}

	VkResult createInstance(const char* appName, uint32_t appVersion, VkInstance *pInstance, const void* pNext) {
		VkApplicationInfo appInfo{};
		appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 3);
		appInfo.applicationVersion = appVersion;
		appInfo.pApplicationName = "MarsEngineVk";

		VkInstanceCreateInfo instanceCreateInfo{};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pApplicationInfo = &appInfo;
		instanceCreateInfo.enabledExtensionCount = instanceExtensions.size();
		instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();
		instanceCreateInfo.enabledLayerCount = instanceLayers.size();
		instanceCreateInfo.ppEnabledLayerNames = instanceLayers.data();
		instanceCreateInfo.pNext = pNext;

		return vkCreateInstance(&instanceCreateInfo, nullptr, pInstance);
	}

	void destroyInstance(VkInstance instance) {
		vkDestroyInstance(instance, nullptr);
	}

	std::vector<VkPhysicalDevice> getPhysicalDevices(VkInstance *pInstance) {
		uint32_t count = 0;
		vkEnumeratePhysicalDevices(*pInstance, &count, nullptr);
		std::vector<VkPhysicalDevice> physicalDevices(count);
		vkEnumeratePhysicalDevices(*pInstance, &count, physicalDevices.data());
		return physicalDevices;
	}

	int32_t getQueueFamilyIndex(VkPhysicalDevice physicalDevice, VkQueueFlags flags) {
		uint32_t count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, nullptr);
		std::vector<VkQueueFamilyProperties> propertiesList(count);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, propertiesList.data());
		for (uint32_t i = 0; i < count; i++) {
			if (propertiesList[i].queueFlags & flags)
				return i;
		}
		return -1;
	}

	VkDeviceQueueCreateInfo generateQueueCreateInfo(uint32_t queueCount, uint32_t queueFamilyIndex, float *priorities) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.pQueuePriorities = priorities;
		queueCreateInfo.queueCount = queueCount;
		queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
		return queueCreateInfo;
	}

	VkResult createDevice(VkPhysicalDevice physicalDevice, VkDeviceQueueCreateInfo *queueCreateInfos, uint32_t queueCount, VkDevice* device) {
		VkDeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.queueCreateInfoCount = queueCount;
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos;
		return vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, device);
	}

	VkResult createDevice(VkPhysicalDevice physicalDevice, std::vector<VkDeviceQueueCreateInfo> queueCreateInfos, VkDevice* device) {
		return createDevice(physicalDevice, queueCreateInfos.data(), (uint32_t) queueCreateInfos.size(), device);
	}

	void destroyDevice(VkDevice device) {
		vkDestroyDevice(device, nullptr);
	}

	

}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t msgCode, const char * pLayerPrefix, const char * pMessage, void * pUserData)
{

	bool error = false;

	std::ostringstream output;
	if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
		error = true;
		output << "[ERROR]";
	}
	else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
		output << "[WARN]";
	}
	else if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
		output << "[PERFORMANCE WARN]";
	}
	else if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) {
		output << "[DEBUG]";
	}
	else if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) {
		output << "[INFO]";
	}

	output << "[" << pLayerPrefix << "] " << pMessage << std::endl;

	if (error) {
		std::cerr << output.str();
#ifdef _WIN32
		MessageBox(NULL, output.str().c_str(), "VULKAN ERROR", 0);
#endif
	}
	else {
		std::cout << output.str();
	}
	return false;
}