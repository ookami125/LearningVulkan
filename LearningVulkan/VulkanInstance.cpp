#include "VulkanInstance.h"
#include "VulkanException.h"
#include <vector>

VulkanInstance::VulkanInstance(std::vector<const char*> desiredExtensions, std::vector<const char*> desiredLayers)
{
	std::vector<const char*> availableDesiredExtensions;
	std::vector<VkExtensionProperties> availableExtensions = GetExtentionProperties();
	for (auto extenstion : desiredExtensions)
	{
		for (auto availableExtension : availableExtensions)
		{
			if (strcmp(availableExtension.extensionName, extenstion) == 0)
			{
				availableDesiredExtensions.push_back(extenstion);
				break;
			}
		}
	}

	printf("Available Extensions:\n");
	for (auto extension : availableExtensions)
		printf("  %s\n", extension.extensionName);

	std::vector<const char*> availableDesiredLayers;
	std::vector<VkLayerProperties> availableLayers = GetLayerProperties();
	for (auto layer : desiredLayers)
	{
		for (auto availableLayer : availableLayers)
		{
			if (strcmp(availableLayer.layerName, layer) == 0)
			{
				availableDesiredLayers.push_back(layer);
				break;
			}
		}
	}

	printf("Available Layers:\n");
	for (auto extension : availableLayers)
		printf("  %s\n", extension.layerName);

	VkApplicationInfo applicationInfo{};
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.pNext = nullptr;
	applicationInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
	applicationInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
	applicationInfo.pApplicationName = "";
	applicationInfo.pEngineName = "Cieric's Game Engine";

	VkInstanceCreateInfo instanceCreateInfo{};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pNext = nullptr;
	instanceCreateInfo.flags = 0;
	instanceCreateInfo.pApplicationInfo = &applicationInfo;
	instanceCreateInfo.enabledExtensionCount = availableDesiredExtensions.size();
	instanceCreateInfo.ppEnabledExtensionNames = availableDesiredExtensions.data();
	instanceCreateInfo.enabledLayerCount = availableDesiredLayers.size();
	instanceCreateInfo.ppEnabledLayerNames = availableDesiredLayers.data();

	if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance) != VK_SUCCESS)
		throw new VulkanException("Failed to create VkInstance!", __LINE__, __FILE__);
}

VulkanInstance::~VulkanInstance()
{
	if (callback != VK_NULL_HANDLE)
	{
		auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
		if (func != nullptr) {
			func(instance, callback, nullptr);
		}
	}
	if (surface) vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyInstance(instance, nullptr);
}

std::vector<VkExtensionProperties> VulkanInstance::GetExtentionProperties()
{
	uint32_t propertyCount;
	vkEnumerateInstanceExtensionProperties(nullptr, &propertyCount, nullptr);
	std::vector<VkExtensionProperties> properties(propertyCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &propertyCount, properties.data());
	return properties;

}

std::vector<VkLayerProperties> VulkanInstance::GetLayerProperties()
{
	uint32_t propertyCount;
	vkEnumerateInstanceLayerProperties(&propertyCount, nullptr);
	std::vector<VkLayerProperties> properties(propertyCount);
	vkEnumerateInstanceLayerProperties(&propertyCount, properties.data());
	return properties;

}

VkPhysicalDevice VulkanInstance::GetPhysicalSuitableDevice()
{
	uint32_t physicalDeviceCount;
	vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
	std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
	vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());
	return physicalDevices[0];
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char* layerPrefix, const char* msg, void* userData) {
	printf("validation layer: %s\n", msg);
	return VK_FALSE;
}

void VulkanInstance::setupDebugCallback() {

	VkDebugReportCallbackCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	createInfo.pfnCallback = debugCallback;

	auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
	if (func != nullptr) {
		if (func(instance, &createInfo, nullptr, &callback) != VK_SUCCESS)
			throw new VulkanException("failed to set up debug callback!", __LINE__, __FILE__);
	}
	else {
		throw new VulkanException("VkDebugReportCallbackCreateInfoEXT doesn't exist!", __LINE__, __FILE__);
	}
}

void VulkanInstance::CreateSurface(HWND hwnd)
{
	VkWin32SurfaceCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.pNext = nullptr;
	createInfo.hwnd = hwnd;
	createInfo.hinstance = GetModuleHandle(nullptr);
	createInfo.flags = 0;

	auto result = vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface);
	if(result != VK_SUCCESS)
		throw new VulkanException("failed to create window surface!", __LINE__, __FILE__);
}

VkSurfaceKHR * VulkanInstance::GetSurface()
{
	return &surface;
}
