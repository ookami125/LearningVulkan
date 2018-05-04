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
	std::array<VkDescriptorPoolSize, 2> poolSizes = {};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = 1;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = 1;

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

//std::vector<VkDescriptorSet> VulkanDescriptorPool::AllocateDescriptorSets(std::vector<VulkanDescriptorSetLayout*> descriptorSetLayouts)
//{
//	std::vector<VkDescriptorSetLayout> descriptorSetLayout(descriptorSetLayouts.size());
//	for (uint32_t i = 0; i < descriptorSetLayouts.size(); ++i)
//		descriptorSetLayout[i] = *descriptorSetLayouts[i];
//
//	VkDescriptorSetAllocateInfo allocInfo = {};
//	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
//	allocInfo.descriptorPool = descriptorPool;
//	allocInfo.descriptorSetCount = descriptorSetLayout.size();
//	allocInfo.pSetLayouts = descriptorSetLayout.data();
//
//	std::vector<VkDescriptorSet> descriptorSets(descriptorSetLayouts.size());
//	if (vkAllocateDescriptorSets(*device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
//		throw std::runtime_error("failed to allocate descriptor set!");
//	}
//
//	return descriptorSets;
//}

VkDescriptorSet VulkanDescriptorPool::AllocateDescriptorSet(VulkanDescriptorSetLayout* descriptorSetLayout, VulkanUniformBufferObject* uniformBufferObject, Texture* texture)
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

	VkDescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer = uniformBufferObject->GetBuffer();
	bufferInfo.offset = 0;
	bufferInfo.range = uniformBufferObject->GetBufferSize();

	VkDescriptorImageInfo imageInfo = {};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = ((VulkanTextureData*)texture->rendererData)->image->GetImageView();
	imageInfo.sampler = *((VulkanTextureData*)texture->rendererData)->sampler;

	std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};

	descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[0].dstSet = descriptorSet;
	descriptorWrites[0].dstBinding = 0;
	descriptorWrites[0].dstArrayElement = 0;
	descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pBufferInfo = &bufferInfo;

	descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[1].dstSet = descriptorSet;
	descriptorWrites[1].dstBinding = 1;
	descriptorWrites[1].dstArrayElement = 0;
	descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrites[1].descriptorCount = 1;
	descriptorWrites[1].pImageInfo = &imageInfo;

	vkUpdateDescriptorSets(*device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

	return descriptorSet;
}
