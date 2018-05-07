#pragma once
#include <vector>
#include <vulkan\vulkan.h>
#include <utility>

class VulkanDevice;

class VulkanDescriptorSetLayout
{
	VulkanDevice* device;

	VkDescriptorSetLayout descriptorSetLayout;
public:
	VulkanDescriptorSetLayout(VulkanDevice* device);
	VulkanDescriptorSetLayout(VulkanDevice* device, std::vector<std::pair<VkDescriptorType, VkShaderStageFlags>> options);
	~VulkanDescriptorSetLayout();

	VkDescriptorSetLayout* GetLayout();

	operator VkDescriptorSetLayout() const;
};