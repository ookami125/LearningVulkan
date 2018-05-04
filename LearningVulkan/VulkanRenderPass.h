#pragma once
#include <vulkan\vulkan.h>

class VulkanSwapchain;
class VulkanDevice;

class VulkanRenderPass
{
	VulkanDevice* device;

	VkRenderPass renderPass;

public:
	VulkanRenderPass(VkPhysicalDevice* physicalDevice, VulkanDevice* device, VulkanSwapchain* swapchain);
	~VulkanRenderPass();

	operator VkRenderPass() const;
};