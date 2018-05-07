#include "VulkanDescriptorSetLayout.h"
#include "VulkanDevice.h"
#include <array>

VkDescriptorSetLayoutBinding VulkanDescriptorSetLayout::Type2Binding(VulkanDescriptorSetLayoutType type, VulkanDescriptorSetLayoutStage stage, uint32_t count)
{
	VkDescriptorSetLayoutBinding layoutBinding = {};
	layoutBinding.binding = count;
	switch (type)
	{
	case VulkanDescriptorSetLayoutType::DSLT_Buffer:
		layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		break;
	case VulkanDescriptorSetLayoutType::DSLT_Sampler:
		layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		break;
	}
	layoutBinding.descriptorCount = 1;
	switch (type)
	{
	case VulkanDescriptorSetLayoutStage::DSLS_Vertex:
		layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		break;
	case VulkanDescriptorSetLayoutStage::DSLS_Fragment:
		layoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		break;
	}

	layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	return layoutBinding;
}

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VulkanDevice* device, std::vector<std::pair<VulkanDescriptorSetLayoutType, VulkanDescriptorSetLayoutStage>> types) : device(device)
{
	std::vector<VkDescriptorSetLayoutBinding> bindings;
	int i = 0;
	for (auto type : types)
		bindings.push_back(Type2Binding(type.first, type.second, i++));
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
