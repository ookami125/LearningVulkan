#include "VulkanDescriptorSetLayout.h"
#include <vector>
#include "VulkanDevice.h"

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VulkanDevice* device) : device(device)
{
	VkDescriptorSetLayoutBinding uboViewProjLayoutBinding = {};
	uboViewProjLayoutBinding.binding = 0;
	uboViewProjLayoutBinding.descriptorCount = 1;
	uboViewProjLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboViewProjLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboViewProjLayoutBinding.pImmutableSamplers = nullptr; // Optional

	VkDescriptorSetLayoutBinding uboModelDataLayoutBinding = {};
	uboModelDataLayoutBinding.binding = 1;
	uboModelDataLayoutBinding.descriptorCount = 1;
	uboModelDataLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboModelDataLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboModelDataLayoutBinding.pImmutableSamplers = nullptr; // Optional

	VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
	samplerLayoutBinding.binding = 2;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding imageLayoutBinding = {};
	imageLayoutBinding.binding = 3;
	imageLayoutBinding.descriptorCount = 1;
	imageLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	imageLayoutBinding.pImmutableSamplers = nullptr;
	imageLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding uboImageDataLayoutBinding = {};
	uboImageDataLayoutBinding.binding = 4;
	uboImageDataLayoutBinding.descriptorCount = 1;
	uboImageDataLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboImageDataLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	uboImageDataLayoutBinding.pImmutableSamplers = nullptr; // Optional

	std::vector<VkDescriptorSetLayoutBinding> bindings = {
		uboViewProjLayoutBinding,
		uboModelDataLayoutBinding,
		samplerLayoutBinding,
		imageLayoutBinding,
		uboImageDataLayoutBinding
	};
	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(*device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VulkanDevice * device, std::vector<VulkanDescriptorSet> options) : device(device)
{
	std::vector<VkDescriptorSetLayoutBinding> bindings(options.size());
	for (uint32_t i = 0; i < options.size(); ++i)
	{
		VkDescriptorSetLayoutBinding& layoutBinding = bindings[i];
		layoutBinding.binding = i;
		layoutBinding.descriptorCount = options[i].count;
		layoutBinding.descriptorType = options[i].type;
		layoutBinding.stageFlags = options[i].shader;
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
