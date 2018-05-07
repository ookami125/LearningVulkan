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

	VkDescriptorPool descriptorPool;
public:
	VulkanDescriptorPool(VulkanDevice* device);
	~VulkanDescriptorPool();

	VkDescriptorSet AllocateDescriptorSet(VulkanDescriptorSetLayout * descriptorSetLayout);
	void UpdateDescriptorSets(VkDescriptorSet descriptorSet, uint32_t binding, VulkanUniformBufferObject * uniformBufferObject);
	void UpdateDescriptorSets(VkDescriptorSet descriptorSet, uint32_t binding, Texture * texture);
};