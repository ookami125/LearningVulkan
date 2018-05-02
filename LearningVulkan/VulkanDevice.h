#pragma once
#include <vulkan/vulkan.h>
#include <vector>

class VulkanDevice
{
	VkDevice device;
public:
	VulkanDevice();
	~VulkanDevice();

	std::vector<VkExtensionProperties> GetExtensionProperties;
};