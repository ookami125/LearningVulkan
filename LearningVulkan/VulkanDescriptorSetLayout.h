#pragma once
#include <vulkan\vulkan.h>
#include <vector>

class VulkanDevice;

enum VulkanDescriptorSetLayoutType
{
	DSLT_Buffer,
	DSLT_Sampler,
};

enum VulkanDescriptorSetLayoutStage
{
	DSLS_Vertex,
	DSLS_Fragment,
};

class VulkanDescriptorSetLayout
{
	VulkanDevice* device;

	VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;

	VkDescriptorSetLayoutBinding Type2Binding(VulkanDescriptorSetLayoutType type, VulkanDescriptorSetLayoutStage stage, uint32_t count);
public:
	VulkanDescriptorSetLayout(VulkanDevice* device, std::vector<std::pair<VulkanDescriptorSetLayoutType, VulkanDescriptorSetLayoutStage>> types);
	~VulkanDescriptorSetLayout();

	VkDescriptorSetLayout* GetLayout();

	operator VkDescriptorSetLayout() const;
};