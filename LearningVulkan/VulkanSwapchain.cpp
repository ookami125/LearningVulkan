#include "VulkanSwapchain.h"
#include "VulkanException.h"
#include "VulkanInstance.h"
#include "VulkanDevice.h"
#include "VulkanImage.h"
#include "VulkanRenderPass.h"
#include "VulkanCommandPool.h"
#include "VulkanUtils.h"
#include <vulkan\vulkan.h>
#include <algorithm>
#include <array>

#include "Logger.h"

VkSurfaceFormatKHR VulkanSwapchain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) {
		return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR VulkanSwapchain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes)
{
	VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
		else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
			bestMode = availablePresentMode;
		}
	}

	return bestMode;
}

VkExtent2D VulkanSwapchain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR & capabilities, uint32_t width, uint32_t height)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	else {
		VkExtent2D actualExtent = { width, height };

		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

VulkanSwapchain::VulkanSwapchain(VkPhysicalDevice* physicalDevice, VulkanInstance* instance, VulkanDevice* logicalDevice, uint32_t width, uint32_t height) : device(logicalDevice)
{
	SwapChainSupportDetails swapChainSupport = instance->QuerySwapChainSupport(physicalDevice);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, width, height);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = *instance->GetSurface();

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = instance->FindQueueFamilies(physicalDevice);
	uint32_t queueFamilyIndices[] = { (uint32_t)indices.graphicsFamily, (uint32_t)indices.presentFamily };

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(*logicalDevice, &createInfo, nullptr, &swapchain) != VK_SUCCESS) {
		LOGGER->Log("%s : %d", __FILE__, __LINE__);
		throw std::runtime_error("failed to create swap chain!");
	}

	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;

	std::vector<VkImage> images;
	vkGetSwapchainImagesKHR(*logicalDevice, swapchain, &imageCount, nullptr);
	images.resize(imageCount);
	vkGetSwapchainImagesKHR(*logicalDevice, swapchain, &imageCount, images.data());	
	swapChainImages.resize(imageCount);

	swapChainDepthImageFormat = FindDepthFormat(physicalDevice);
	depthImage = new VulkanImage(device, physicalDevice, width, height, swapChainDepthImageFormat, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	for (uint32_t i=0; i<imageCount; ++i)
	{
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.image = images[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = swapChainImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		VkImageView view;
		auto result = vkCreateImageView(*logicalDevice, &createInfo, nullptr, &view);
		if (result != VK_SUCCESS) {
			LOGGER->Log("%s : %d", __FILE__, __LINE__);
			throw new VulkanException("failed to create image view!", __LINE__, __FILE__);
		}

		swapChainImages[i] = new VulkanImage(logicalDevice, images[i], view, true);
	}
}

VulkanSwapchain::~VulkanSwapchain()
{
	for (auto framebuffer : swapChainFramebuffers) {
		vkDestroyFramebuffer(*device, framebuffer, nullptr);
	}
	delete depthImage;
	for (VulkanImage* image : swapChainImages)
	{
		delete image;
		image = nullptr;
	}
	swapChainImages.clear();
	vkDestroySwapchainKHR(*device, swapchain, nullptr);
}

void VulkanSwapchain::InitFrameBuffers(VulkanRenderPass* renderpass)
{
	swapChainFramebuffers.resize(swapChainImages.size());
	for (size_t i = 0; i < swapChainImages.size(); i++) {
		std::array<VkImageView, 2> attachments = {
			swapChainImages[i]->GetImageView(),
			depthImage->GetImageView()
		};

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = *renderpass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(*device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
			LOGGER->Log("%s : %d", __FILE__, __LINE__);
			throw new VulkanException("failed to create framebuffer!", __LINE__, __FILE__);
		}
	}
}

void VulkanSwapchain::InitCommandBuffers(VulkanCommandPool * commandPool)
{

	commandBuffers.resize(swapChainFramebuffers.size());

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = *commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	if (vkAllocateCommandBuffers(*device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
		LOGGER->Log("%s : %d", __FILE__, __LINE__);
		throw new VulkanException("failed to allocate command buffers!", __LINE__, __FILE__);
	}

	depthImage->TransitionImageLayout(commandPool, swapChainDepthImageFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

VkFormat VulkanSwapchain::GetImageFormat()
{
	return swapChainImageFormat;
}

void VulkanSwapchain::NextFrame()
{
	frameCounter = frameCounter + 1;
	if (frameCounter >= commandBuffers.size())
		frameCounter = 0;
}

uint32_t VulkanSwapchain::GetFrameCount()
{
	return commandBuffers.size();
}

uint32_t VulkanSwapchain::GetCurrentFrameIndex()
{
	return frameCounter;
}

VkCommandBuffer VulkanSwapchain::GetNextCommandBuffer()
{
	return commandBuffers[frameCounter];
}

VkFramebuffer VulkanSwapchain::GetNextFrameBuffer()
{
	return swapChainFramebuffers[frameCounter];
}

VulkanImage* VulkanSwapchain::GetImage(uint32_t idx)
{
	return swapChainImages[idx];
}

VkExtent2D VulkanSwapchain::GetExtent()
{
	return swapChainExtent;
}

VkFormat VulkanSwapchain::GetFormat()
{
	return swapChainImageFormat;
}

VulkanSwapchain::operator VkSwapchainKHR() const
{
	return swapchain;
}

