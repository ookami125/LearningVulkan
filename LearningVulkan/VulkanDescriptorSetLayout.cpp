#include "VulkanDescriptorSetLayout.h"
#include <vector>
#include "VulkanDevice.h"
#include "Logger.h"

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VulkanDevice* device) : device(device)
{
	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

	VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;


	std::vector<VkDescriptorSetLayoutBinding> bindings = { uboLayoutBinding, samplerLayoutBinding };
	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(*device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
		LOGGER->Log("%s : %d", __FILE__, __LINE__);
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VulkanDevice * device, std::vector<std::pair<VkDescriptorType, VkShaderStageFlags>> options) : device(device)
{
	std::vector<VkDescriptorSetLayoutBinding> bindings(options.size());
	for (uint32_t i = 0; i < options.size(); ++i)
	{
		VkDescriptorSetLayoutBinding& layoutBinding = bindings[i];
		layoutBinding.binding = i;
		layoutBinding.descriptorCount = 1;
		layoutBinding.descriptorType = options[i].first;
		layoutBinding.stageFlags = options[i].second;
		layoutBinding.pImmutableSamplers = nullptr; // Optional
	}
	
	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();
	
	if (vkCreateDescriptorSetLayout(*device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
{
	vkDestroyDescriptorSetLayout(*device, descriptorSetLayout, nullptr);
}

VkDescriptorSetLayout * VulkanDescriptorSetLayout::GetLayout()
{
	return &descriptorSetLayout;
}

VulkanDescriptorSetLayout::operator VkDescriptorSetLayout() const
{
	return descriptorSetLayout;
}
