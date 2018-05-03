#include "VulkanException.h"
#include "VulkanRenderer.h"
#include "VulkanInstance.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanShaderStage.h"
#include "VulkanShader.h"
#include "VulkanPipeline.h"
#include "VulkanRenderPass.h"
#include "VulkanCommandPool.h"
#include <vector>

VulkanRenderer::VulkanRenderer(HWND hwnd)
{
	RECT rect;
	GetClientRect(hwnd, &rect);
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;
	instance = new VulkanInstance(hwnd, { "VK_KHR_surface", VK_EXT_DEBUG_REPORT_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME }, { "VK_LAYER_LUNARG_standard_validation" });
	VkPhysicalDevice physicalDevice = instance->GetPhysicalSuitableDevice({ VK_KHR_SWAPCHAIN_EXTENSION_NAME  });
	device = new VulkanDevice(&physicalDevice, instance, { VK_KHR_SWAPCHAIN_EXTENSION_NAME }, { "VK_LAYER_LUNARG_standard_validation" });
	swapchain = new VulkanSwapchain(&physicalDevice, instance, device, width, height);
	shaderStage = new VulkanShaderStage(device, { "shaders/shader_base.vert.spv", "shaders/shader_base.frag.spv" });
	renderPass = new VulkanRenderPass(device, swapchain);
	pipeline = new VulkanPipeline(device, swapchain, shaderStage, renderPass);
	swapchain->InitFrameBuffers(renderPass);
	commandPool = new VulkanCommandPool(&physicalDevice, device, instance);
	swapchain->InitCommandBuffers(commandPool);

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	if (vkCreateSemaphore(*device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(*device, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS) {

		throw std::runtime_error("failed to create semaphores!");
	}

	for (size_t i = 0; i < swapchain->commandBuffers.size(); i++) {
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		beginInfo.pInheritanceInfo = nullptr; // Optional

		if (vkBeginCommandBuffer(swapchain->commandBuffers[i], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = *renderPass;
		renderPassInfo.framebuffer = swapchain->swapChainFramebuffers[i];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapchain->GetExtent();

		VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(swapchain->commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(swapchain->commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, *pipeline);

		vkCmdDraw(swapchain->commandBuffers[i], 3, 1, 0, 0);

		vkCmdEndRenderPass(swapchain->commandBuffers[i]);

		if (vkEndCommandBuffer(swapchain->commandBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}
}

VulkanRenderer::~VulkanRenderer()
{
	vkDestroySemaphore(*device, renderFinishedSemaphore, nullptr);
	vkDestroySemaphore(*device, imageAvailableSemaphore, nullptr);
	delete commandPool;
	delete pipeline;
	delete renderPass;
	delete shaderStage;
	delete swapchain;
	delete device;
	delete instance;
}

void VulkanRenderer::Render()
{
	uint32_t imageIndex;
	vkAcquireNextImageKHR(*device, *swapchain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &swapchain->commandBuffers[imageIndex];

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(device->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { *swapchain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &imageIndex;

	vkQueuePresentKHR(device->GetPresentQueue(), &presentInfo);

	vkQueueWaitIdle(device->GetPresentQueue());
}

