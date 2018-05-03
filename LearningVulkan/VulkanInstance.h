#pragma once
#include <Windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan\vulkan.h>
#include <vector>

class VulkanPhysicalDevice;

struct QueueFamilyIndices {
	int graphicsFamily = -1;
	int presentFamily = -1;

	bool isComplete() {
		return graphicsFamily >= 0 && presentFamily >= 0;
	}
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class VulkanInstance
{
	VkInstance instance = VK_NULL_HANDLE;
	VkDebugReportCallbackEXT callback = VK_NULL_HANDLE;
	VkSurfaceKHR surface = VK_NULL_HANDLE;

	bool CheckDeviceExtensionSupport(VkPhysicalDevice* device, const std::vector<const char*> deviceExtensions = {});

public:
	VulkanInstance(HWND hwnd, std::vector<const char*> desiredExtensions = {}, std::vector<const char*> desiredLayers = {});
	~VulkanInstance();
	static std::vector<VkExtensionProperties> GetExtentionProperties();
	static std::vector<VkLayerProperties> GetLayerProperties();
	VkPhysicalDevice GetPhysicalSuitableDevice(const std::vector<const char*> deviceExtensions = {});
	void SetupDebugCallback();

	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice* device);
	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice* device);

	VkSurfaceKHR* GetSurface();
};