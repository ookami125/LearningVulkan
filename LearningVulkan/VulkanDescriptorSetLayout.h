#pragma once
#include <vector>
#include <vulkan\vulkan.h>
#include <utility>

class VulkanDevice;

struct VulkanDescriptorSet
{
	int count = 1;
	VkDescriptorType type;
	VkShaderStageFlags shader;
};

class VulkanDescriptorSetLayout
{
	VulkanDevice* device;

	VkDescriptorSetLayout descriptorSetLayout;
public:
	VulkanDescriptorSetLayout(VulkanDevice* device);
	VulkanDescriptorSetLayout(VulkanDevice* device, std::vector<VulkanDescriptorSet> options);
	~VulkanDescriptorSetLayout();

	VkDescriptorSetLayout* GetLayout();

	operator VkDescriptorSetLayout() const;
};