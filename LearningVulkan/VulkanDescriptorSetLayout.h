#pragma once
#include <vulkan\vulkan.h>

class VulkanDevice;

class VulkanDescriptorSetLayout
{
	VulkanDevice* device;

	VkDescriptorSetLayout descriptorSetLayout;
public:
	VulkanDescriptorSetLayout(VulkanDevice* device);
	~VulkanDescriptorSetLayout();

	VkDescriptorSetLayout* GetLayout();

	operator VkDescriptorSetLayout() const;
};