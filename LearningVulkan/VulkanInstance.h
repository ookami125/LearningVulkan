#pragma once
#include <vulkan\vulkan.h>
#include <vector>

class VulkanInstance
{
	VkInstance instance;

public:
	VulkanInstance(std::vector<const char*> desiredExtensions = {}, std::vector<const char*> desiredLayers = {});
	~VulkanInstance();
	static std::vector<VkExtensionProperties> GetExtentionProperties();
	static std::vector<VkLayerProperties> GetLayerProperties();
};