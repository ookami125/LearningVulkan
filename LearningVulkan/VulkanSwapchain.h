#pragma once

#include "VulkanDevice.h"
#include "VulkanCommandBuffer.h"
#include "VulkanRenderpass.h"
#include <GLFW\glfw3.h>

class VulkanSwapchain
{
private:
	struct SwapChainBuffer
	{
		VkImage image;
		VkImageView view;
	};
	VkSwapchainKHR swapChain;
	std::vector<SwapChainBuffer> swapChainBuffers;
	uint32_t currentBuffer = 0;
	VkFramebuffer * frameBuffers;
public:
	VulkanSwapchain();
	~VulkanSwapchain();

	bool Init(GLFWwindow* window, VulkanDevice * vulkanDevice, VkImageView depthImageView, VulkanRenderpass * vulkanRenderpass);
	void Unload(VulkanDevice * vulkanDevice);
	void AcquireNextImage(VulkanDevice * vulkanDevice, VkSemaphore signalSemaphore);
	void Present(VulkanDevice * vulkanDevice, VkSemaphore waitSemaphore);
	VkImage GetCurrentImage();
	uint32_t GetCurrentBufferId();
	VkFramebuffer GetFramebuffer(int id);
	size_t GetSwapchainBufferCount();
};