#pragma once
#include <vulkan\vulkan.h>
#include <vector>

class VulkanDevice;
class VulkanSwapchain;
class VulkanShaderStage;
class VulkanRenderPass;
class VulkanDescriptorSetLayout;

class VulkanPipeline
{
	VulkanDevice* device;

	VkPipeline pipeline;
	VkPipelineLayout pipelineLayout;
public:
	VulkanPipeline(VulkanDevice* device, VulkanSwapchain* swapchain, VulkanShaderStage* shaderStage, VulkanRenderPass* renderPass, std::vector<VulkanDescriptorSetLayout*> descriptorSetLayouts = {});
	~VulkanPipeline();

	VkPipelineLayout GetLayout();

	operator VkPipeline() const;
};