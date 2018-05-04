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

	VkFormat swapChainDepthImageFormat;
	VulkanImage* depthImage;

	uint32_t frameCounter = 0;
	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<VkFramebuffer> swapChainFramebuffers;

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR & capabilities, uint32_t width, uint32_t height);
public:

	VulkanSwapchain(VkPhysicalDevice* physicalDevice, VulkanInstance* instance, VulkanDevice* logicalDevice, uint32_t width, uint32_t height);
	~VulkanSwapchain();

	void InitFrameBuffers(VulkanRenderPass* renderpass);
	void InitCommandBuffers(VulkanCommandPool* commandPool);

	VkFormat GetImageFormat();

	void NextFrame();
	VkCommandBuffer GetNextCommandBuffer();
	VkFramebuffer GetNextFrameBuffer();
	VulkanImage* GetImage(uint32_t idx);

	VkExtent2D GetExtent();
	VkFormat GetFormat();

	operator VkSwapchainKHR() const;
};