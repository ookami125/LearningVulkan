#pragma once
#include <vulkan\vulkan.h>

class VulkanDevice;

class VulkanImageSampler
{
	VulkanDevice* device;

	VkSampler sampler;
public:
	VulkanImageSampler(VulkanDevice* device);
	~VulkanImageSampler();

	operator VkSampler() const;
};