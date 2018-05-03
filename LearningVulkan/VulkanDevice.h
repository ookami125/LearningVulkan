#pragma once
#include <vulkan/vulkan.h>
#include <vector>

class VulkanInstance;

class VulkanDevice
{
	VkDevice device = VK_NULL_HANDLE;
	VkQueue deviceGraphicsQueue = VK_NULL_HANDLE;
	VkQueue devicePresentQueue = VK_NULL_HANDLE;
public:
	VulkanDevice(VkPhysicalDevice * physicalDevice, VulkanInstance* instance, std::vector<const char*> desiredExtensions, std::vector<const char*> desiredLayers);
	~VulkanDevice();

	VkQueue GetGraphicsQueue();
	VkQueue GetPresentQueue();

	operator VkDevice() const;

	//static std::vector<VkExtensionProperties> GetExtensionProperties();
};