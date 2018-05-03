#pragma once
#include <vulkan\vulkan.h>

class VulkanDevice;
class VulkanSwapchain;
class VulkanShaderStage;
class VulkanRenderPass;

class VulkanPipeline
{
	VulkanDevice* device;

	VkPipeline pipeline;
	VkPipelineLayout pipelineLayout;
public:
	VulkanPipeline(VulkanDevice* device, VulkanSwapchain* swapchain, VulkanShaderStage* shaderStage, VulkanRenderPass* renderPass);
	~VulkanPipeline();

	operator VkPipeline() const;
};