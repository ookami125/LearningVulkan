#pragma once

#define VULKAN_DEBUG_MODE_ENABLED false

#define VK_USE_PLATFORM_WIN32_KHR

#include <GLFW\glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vulkan\vulkan.h>

#include "VulkanInstance.h"
#include "VulkanDevice.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"
//#include "VulkanSwapchain.h"
#include "VulkanRenderpass.h"
//#include "FrameBufferAttachment.h"

class VulkanRenderer
{
private:
	struct
	{
		VkFormat format;
		VkImage image;
		VkDeviceMemory mem;
		VkImageView view;
	} depthImage;

	VulkanInstance * vulkanInstance;
	VulkanDevice * vulkanDevice;
	VulkanCommandPool * vulkanCommandPool;
	VulkanCommandBuffer * initCommandBuffer;
	//VulkanSwapchain * vulkanSwapchain;
	VulkanRenderpass * forwardRenderPass;
	VulkanRenderpass * deferredRenderPass;

	VkViewport viewport;
	VkRect2D scissor;

	VkSampler colorSampler;
	VkFramebuffer deferredFramebuffer;

	//FrameBufferAttachment * positionAtt;
	//FrameBufferAttachment * normalAtt;
	//FrameBufferAttachment * albedoAtt;
	//FrameBufferAttachment * materialAtt;
	//FrameBufferAttachment * depthAtt;
	//std::vector<FrameBufferAttachment*> attachmentsPtr;

	VkSemaphore imageReadySemaphore;
	VkSemaphore drawCompleteSemaphore;

	VkPipelineCache pipelineCache;

	VkDebugReportCallbackEXT debugReport;

	GLFWwindow* window;
private:
	bool InitDepthBuffer();
	bool InitColorSampler();
	bool InitDeferredFramebuffer();

	bool InitVulkanDebugMode();
	void UnloadVulkanDebugMode();

public:
	VulkanRenderer();
	~VulkanRenderer();

	bool Init(GLFWwindow* window, bool debugMode = false);
	//void BeginSceneDeferred(VulkanCommandBuffer * commandBuffer);
	//void EndSceneDeferred(VulkanCommandBuffer * commandBuffer);
	//void BeginSceneForward(VulkanCommandBuffer * commandBuffer, int frameId);
	//void EndSceneForward(VulkanCommandBuffer * commandBuffer);
	//void Present(std::vector<VulkanCommandBuffer*>& renderCommandBuffers);
	//void InitViewportAndScissors(VulkanCommandBuffer * commandBuffer, float vWidth, float vHeight, uint32_t sWidth, uint32_t sHeight);
	//VulkanCommandPool * GetVulkanCommandPool();
	//VulkanDevice * GetVulkanDevice();
	//VulkanRenderpass * GetForwardRenderpass();
	//VulkanRenderpass * GetDeferredRenderpass();
	//VulkanSwapchain * GetVulkanSwapchain();
	//VkSampler GetColorSampler();
	//FrameBufferAttachment * GetPositionAttachment();
	//FrameBufferAttachment * GetNormalAttachment();
	//FrameBufferAttachment * GetAlbedoAttachment();
	//FrameBufferAttachment * GetMaterialAttachment();
	//FrameBufferAttachment * GetDepthAttachment();
	//VkFramebuffer GetDeferredFramebuffer();
	//VkPipelineCache GetPipelineCache();
};