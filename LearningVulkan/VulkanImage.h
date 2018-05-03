#pragma once
#include <vulkan\vulkan.h>

class VulkanDevice;

class VulkanImage
{
	VulkanDevice* device;

	VkImage image;
	VkImageView view;

	bool swapchainOwned;
public:
	VulkanImage(VulkanDevice* device, VkImage image, VkImageView view, bool swapchainOwned = false);
	~VulkanImage();

	VkImageView GetImageView();
};