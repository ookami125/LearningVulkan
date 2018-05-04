#pragma once
#include <vulkan\vulkan.h>

class VulkanDevice;
class VulkanCommandPool;

class VulkanImage
{
	VulkanDevice* device;

	VkImage image = VK_NULL_HANDLE;
	VkDeviceMemory imageMemory = VK_NULL_HANDLE;
	VkImageView view = VK_NULL_HANDLE;

	bool swapchainOwned;
public:
	VulkanImage(VulkanDevice* device, VkImage image, VkImageView view, bool swapchainOwned = false);
	VulkanImage(VulkanDevice* device, VkPhysicalDevice* physicalDevice, uint32_t width, uint32_t height, VkFormat format, VkImageAspectFlags aspectFlags, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
	VulkanImage(VulkanDevice * device, VkPhysicalDevice * physicalDevice, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
	~VulkanImage();

	void CopyFromBuffer(VkBuffer buffer, uint32_t width, uint32_t height, VulkanDevice * device, VulkanCommandPool * commandPool);

	void TransitionImageLayout(VulkanCommandPool* commandPool, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

	VkImageView GetImageView();
};