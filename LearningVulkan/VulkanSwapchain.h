#pragma once
#include <vulkan\vulkan.h>
#include <vector>

class VulkanInstance;
class VulkanDevice;
class VulkanImage;
class VulkanRenderPass;
class VulkanCommandPool;

class VulkanSwapchain
{
	VulkanDevice* device;

	VkSwapchainKHR swapchain;
	std::vector<VulkanImage*> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR & capabilities, uint32_t width, uint32_t height);
public:
	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<VkFramebuffer> swapChainFramebuffers;

	VulkanSwapchain(VkPhysicalDevice* physicalDevice, VulkanInstance* instance, VulkanDevice* logicalDevice, uint32_t width, uint32_t height);
	~VulkanSwapchain();

	void InitFrameBuffers(VulkanRenderPass* renderpass);
	void InitCommandBuffers(VulkanCommandPool* commandPool);

	VkExtent2D GetExtent();
	VkFormat GetFormat();

	operator VkSwapchainKHR() const;
};