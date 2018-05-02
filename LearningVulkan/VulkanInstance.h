#pragma once
#include <Windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan\vulkan.h>
#include <vector>

class VulkanPhysicalDevice;

class VulkanInstance
{
	VkInstance instance = VK_NULL_HANDLE;
	VkDebugReportCallbackEXT callback = VK_NULL_HANDLE;
	VkSurfaceKHR surface = VK_NULL_HANDLE;

public:
	VulkanInstance(std::vector<const char*> desiredExtensions = {}, std::vector<const char*> desiredLayers = {});
	~VulkanInstance();
	static std::vector<VkExtensionProperties> GetExtentionProperties();
	static std::vector<VkLayerProperties> GetLayerProperties();
	VkPhysicalDevice GetPhysicalSuitableDevice();
	void setupDebugCallback();
	void CreateSurface(HWND hwnd);
	VkSurfaceKHR* GetSurface();
};