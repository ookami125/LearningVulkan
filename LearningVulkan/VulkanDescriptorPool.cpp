#include "VulkanDescriptorPool.h"
#include "VulkanDescriptorSetLayout.h"
#include "VulkanUniformBufferObject.h"
#include "VulkanDevice.h"
#include "VulkanTextureData.h"
#include "VulkanImage.h"
#include "VulkanImageSampler.h"
#include "Texture.h"
#include <array>

VulkanDescriptorPool::VulkanDescriptorPool(VulkanDevice* device) : device(device)
{
	std::array<VkDescriptorPoolSize, 3> poolSizes = {};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = 3;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	poolSizes[1].descriptorCount = 3;
	poolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[2].descriptorCount = 2;

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = 1;

	if (vkCreateDescriptorPool(*device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

VulkanDescriptorPool::~VulkanDescriptorPool()
{
	vkDestroyDescriptorPool(*device, descriptorPool, nullptr);
}

VkDescriptorSet VulkanDescriptorPool::AllocateDescriptorSet(VulkanDescriptorSetLayout* descriptorSetLayout)
{
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = descriptorSetLayout->GetLayout();

	VkDescriptorSet descriptorSet;
	if (vkAllocateDescriptorSets(*device, &allocInfo, &descriptorSet) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor set!");
	}

	return descriptorSet;
}

void VulkanDescriptorPool::UpdateDescriptorSets(VkDescriptorSet descriptorSet, uint32_t binding, VulkanUniformBufferObject* uniformBufferObject)
{
	VkDescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer = uniformBufferObject->GetBuffer();
	bufferInfo.offset = 0;
	bufferInfo.range = uniformBufferObject->GetUBOSize();

	VkWriteDescriptorSet descriptorWrites = {};
	descriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites.dstSet = descriptorSet;
	descriptorWrites.dstBinding = binding;
	descriptorWrites.dstArrayElement = 0;
	descriptorWrites.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	if(uniformBufferObject->isDynamic())
		descriptorWrites.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	descriptorWrites.descriptorCount = 1;
	descriptorWrites.pBufferInfo = &bufferInfo;

	vkUpdateDescriptorSets(*device, 1, &descriptorWrites, 0, nullptr);

}

void VulkanDescriptorPool::UpdateDescriptorSets(VkDescriptorSet descriptorSet, uint32_t binding, Texture* texture, uint32_t count)
{
	VkDescriptorImageInfo imageInfo = {};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = ((VulkanTextureData*)texture->rendererData)->image->GetImageView();
	imageInfo.sampler = *((VulkanTextureData*)texture->rendererData)->sampler;

	VkWriteDescriptorSet descriptorWrites = {};
	descriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites.dstSet = descriptorSet;
	descriptorWrites.dstBinding = binding;
	descriptorWrites.dstArrayElement = 0;
	descriptorWrites.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrites.descriptorCount = count;
	descriptorWrites.pImageInfo = &imageInfo;

	vkUpdateDescriptorSets(*device, 1, &descriptorWrites, 0, nullptr);

}
