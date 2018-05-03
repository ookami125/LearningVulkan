#pragma once
#include <vulkan\vulkan.h>

class VulkanInstance;
class VulkanDevice;

class VulkanCommandPool
{
	VulkanDevice* device;

	VkCommandPool commandPool;
public:
	VulkanCommandPool(VkPhysicalDevice* physicalDevice, VulkanDevice* device, VulkanInstance* instance);
	~VulkanCommandPool();

	operator VkCommandPool() const;
};