#pragma once
#include <vulkan\vulkan.h>
#include <vector>

class VulkanDevice;
class VulkanDescriptorSetLayout;
class VulkanUniformBufferObject;
class VulkanDynamicUBO;
struct Texture;

class VulkanDescriptorPool
{
	VulkanDevice* device;

	VkDescriptorPool descriptorPool;
public:
	VulkanDescriptorPool(VulkanDevice* device);
	~VulkanDescriptorPool();

	VkDescriptorSet AllocateDescriptorSet(VulkanDescriptorSetLayout * descriptorSetLayout);
	std::vector<VkDescriptorSet> AllocateDescriptorSets(VulkanDescriptorSetLayout * descriptorSetLayout, uint32_t count);
	void UpdateDescriptorSets(VkDescriptorSet descriptorSet, uint32_t binding, VulkanUniformBufferObject * uniformBufferObject);
	void UpdateDescriptorSets(VkDescriptorSet descriptorSet, uint32_t binding, VulkanDynamicUBO * uniformBufferObject);
	void UpdateDescriptorSets(VkDescriptorSet descriptorSet, uint32_t binding, VkDescriptorImageInfo * descriptorImageInfos, uint32_t count);
	void UpdateDescriptorSetsSampler(VkDescriptorSet descriptorSet, uint32_t binding, VkDescriptorImageInfo * descriptorImageInfos, uint32_t count);
	void UpdateDescriptorSets(VkDescriptorSet descriptorSet, uint32_t binding, Texture* texture, uint32_t count);
	void UpdateDescriptorSets(VkDescriptorSet descriptorSet, uint32_t binding, std::vector<Texture*> textures);
};