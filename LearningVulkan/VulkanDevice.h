#pragma once
#include <vulkan/vulkan.h>
#include <vector>

struct QueueFamilyIndices {
	int graphicsFamily = -1;
	int presentFamily = -1;

	bool isComplete() {
		return graphicsFamily >= 0 && presentFamily >= 0;
	}
};

class VulkanDevice
{
	VkDevice device = VK_NULL_HANDLE;
	VkQueue deviceQueue = VK_NULL_HANDLE;
public:
	VulkanDevice(VkPhysicalDevice * physicalDevice, VkSurfaceKHR * surface, std::vector<const char*> desiredExtensions, std::vector<const char*> desiredLayers);
	~VulkanDevice();

	//static std::vector<VkExtensionProperties> GetExtensionProperties();
};