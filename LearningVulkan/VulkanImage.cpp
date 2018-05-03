#include "VulkanImage.h"
#include "VulkanDevice.h"
#include <vulkan\vulkan.h>

VulkanImage::VulkanImage(VulkanDevice* device, VkImage image, VkImageView view, bool swapchainOwned) : device(device), image(image), view(view), swapchainOwned(swapchainOwned){}

VulkanImage::~VulkanImage()
{
	vkDestroyImageView(*device, view, nullptr);
	if(!swapchainOwned)
		vkDestroyImage(*device, image, nullptr);
}

VkImageView VulkanImage::GetImageView()
{
	return view;
}
