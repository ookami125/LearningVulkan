#pragma once
#include <vulkan\vulkan.h>
#include <vector>

class VulkanDevice;
class VulkanDescriptorSetLayout;
class VulkanUniformBufferObject;
struct Texture;

class VulkanDescriptorPool
{
	VulkanDevice* device;

	VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
public:
	VulkanDescriptorPool(VulkanDevice* device);
	~VulkanDescriptorPool();

	//std::vector<VkDescriptorSet> AllocateDescriptorSets(std::vector<VulkanDescriptorSetLayout*> descriptorSetLayouts = {});
	VkDescriptorSet AllocateDescriptorSet(VulkanDescriptorSetLayout* descriptorSetLayout, VulkanUniformBufferObject* uniformBufferObject, Texture* texture);
};