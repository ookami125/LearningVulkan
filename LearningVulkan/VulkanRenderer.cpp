#include "VulkanRenderer.h"
#include "VulkanUtils.h"

bool VulkanRenderer::InitDepthBuffer()
{
	VkResult result;
	VkImageCreateInfo imageCI{};

	std::vector<VkFormat> depthFormats;
	depthFormats.push_back(VK_FORMAT_D32_SFLOAT);
	depthFormats.push_back(VK_FORMAT_D16_UNORM);

	VkFormatProperties properties;
	for (unsigned int i = 0; i < depthFormats.size(); i++)
	{
		vkGetPhysicalDeviceFormatProperties(vulkanDevice->GetGPU(), depthFormats[i], &properties);
		if (properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
		{
			depthImage.format = depthFormats[i];
			break;
		}
	}

	vkGetPhysicalDeviceFormatProperties(vulkanDevice->GetGPU(), depthImage.format, &properties);
	if (!(properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT))
	{
		printf("ERROR: Couldn't find a depth image format!\n");
		return false;
	}

	imageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCI.imageType = VK_IMAGE_TYPE_2D;
	imageCI.format = depthImage.format;
	imageCI.tiling = VK_IMAGE_TILING_OPTIMAL;
	
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	imageCI.extent.width = width;
	imageCI.extent.height = height;
	
	imageCI.extent.depth = 1;
	imageCI.mipLevels = 1;
	imageCI.arrayLayers = 1;
	imageCI.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageCI.queueFamilyIndexCount = 0;
	imageCI.pQueueFamilyIndices = VK_NULL_HANDLE;
	imageCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCI.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	imageCI.flags = 0;

	VkMemoryAllocateInfo memAlloc{};
	memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

	VkImageViewCreateInfo viewCI{};
	viewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewCI.image = VK_NULL_HANDLE;
	viewCI.format = depthImage.format;
	viewCI.components.r = VK_COMPONENT_SWIZZLE_R;
	viewCI.components.g = VK_COMPONENT_SWIZZLE_G;
	viewCI.components.b = VK_COMPONENT_SWIZZLE_B;
	viewCI.components.a = VK_COMPONENT_SWIZZLE_A;
	viewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	viewCI.subresourceRange.baseMipLevel = 0;
	viewCI.subresourceRange.levelCount = 1;
	viewCI.subresourceRange.baseArrayLayer = 0;
	viewCI.subresourceRange.layerCount = 1;
	viewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewCI.flags = 0;

	VkMemoryRequirements memReq;
	result = vkCreateImage(vulkanDevice->GetDevice(), &imageCI, VK_NULL_HANDLE, &depthImage.image);
	if (result != VK_SUCCESS)
		return false;

	vkGetImageMemoryRequirements(vulkanDevice->GetDevice(), depthImage.image, &memReq);
	memAlloc.allocationSize = memReq.size;
	if (!vulkanDevice->MemoryTypeFromProperties(memReq.memoryTypeBits, 0, &memAlloc.memoryTypeIndex))
		return false;

	result = vkAllocateMemory(vulkanDevice->GetDevice(), &memAlloc, VK_NULL_HANDLE, &depthImage.mem);
	if (result != VK_SUCCESS)
		return false;

	result = vkBindImageMemory(vulkanDevice->GetDevice(), depthImage.image, depthImage.mem, 0);
	if (result != VK_SUCCESS)
		return false;

	SetImageLayout(depthImage.image, viewCI.subresourceRange.aspectMask, VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, NULL, initCommandBuffer, vulkanDevice, true);

	viewCI.image = depthImage.image;
	result = vkCreateImageView(vulkanDevice->GetDevice(), &viewCI, VK_NULL_HANDLE, &depthImage.view);
	if (result != VK_SUCCESS)
		return false;

	return true;
}

VulkanRenderer::VulkanRenderer()
{
	vulkanInstance = NULL;
	vulkanDevice = NULL;
	vulkanCommandPool = NULL;
	initCommandBuffer = NULL;
	//vulkanSwapchain = NULL;
	forwardRenderPass = NULL;
	deferredRenderPass = NULL;
	//
	//positionAtt = NULL;
	//normalAtt = NULL;
	//albedoAtt = NULL;
	//materialAtt = NULL;
	//depthAtt = NULL;
}

VulkanRenderer::~VulkanRenderer()
{
#if VULKAN_DEBUG_MODE_ENABLED
	UnloadVulkanDebugMode();
#endif
	//vkDestroyPipelineCache(vulkanDevice->GetDevice(), pipelineCache, VK_NULL_HANDLE);
	//
	//vkDestroySemaphore(vulkanDevice->GetDevice(), drawCompleteSemaphore, VK_NULL_HANDLE);
	//vkDestroySemaphore(vulkanDevice->GetDevice(), imageReadySemaphore, VK_NULL_HANDLE);
	//
	//vkDestroyFramebuffer(vulkanDevice->GetDevice(), deferredFramebuffer, VK_NULL_HANDLE);
	SafeUnload(deferredRenderPass, vulkanDevice);
	//SafeUnload(depthAtt, vulkanDevice);
	//SafeUnload(materialAtt, vulkanDevice);
	//SafeUnload(albedoAtt, vulkanDevice);
	//SafeUnload(normalAtt, vulkanDevice);
	//SafeUnload(positionAtt, vulkanDevice);
	//
	vkDestroySampler(vulkanDevice->GetDevice(), colorSampler, VK_NULL_HANDLE);
	vkFreeMemory(vulkanDevice->GetDevice(), depthImage.mem, VK_NULL_HANDLE); depthImage.mem = VK_NULL_HANDLE;
	vkDestroyImage(vulkanDevice->GetDevice(), depthImage.image, VK_NULL_HANDLE); depthImage.image = VK_NULL_HANDLE;
	vkDestroyImageView(vulkanDevice->GetDevice(), depthImage.view, VK_NULL_HANDLE); depthImage.view = VK_NULL_HANDLE;
	//
	//SafeUnload(vulkanSwapchain, vulkanDevice);
	SafeUnload(forwardRenderPass, vulkanDevice);
	SafeUnload(initCommandBuffer, vulkanDevice, vulkanCommandPool);
	SafeUnload(vulkanCommandPool, vulkanDevice);
	SafeUnload(vulkanDevice, vulkanInstance);
	SafeDelete(vulkanInstance);
}

bool VulkanRenderer::Init(GLFWwindow* window, bool debugMode)
{
	this->window = window;
	vulkanInstance = new VulkanInstance();
	if (debugMode)
	{
		vulkanInstance->AddInstanceLayer("VK_LAYER_LUNARG_standard_validation");
		vulkanInstance->AddInstanceExtension(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}
	vulkanInstance->AddInstanceExtension(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
	vulkanInstance->AddInstanceExtension(VK_KHR_SURFACE_EXTENSION_NAME);

	if (!vulkanInstance->Init())
	{
		printf("ERROR: Failed to init vulkan instance!");
		return false;
	}

	if (debugMode)
	{
		//if (!InitVulkanDebugMode())
		//{
		//	printf("ERROR: Failed to init vulkan debug mode!");
		//	return false;
		//}
	}

	vulkanDevice = new VulkanDevice();
	vulkanDevice->AddDeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	if (!vulkanDevice->Init(vulkanInstance, glfwGetWin32Window(window)))
	{
		printf("ERROR: Failed to init vulkan device!");
		return false;
	}
	
	vulkanCommandPool = new VulkanCommandPool();
	if (!vulkanCommandPool->Init(vulkanDevice))
	{
		printf("ERROR: Failed to init command pool!\n");
		return false;
	}
	
	initCommandBuffer = new VulkanCommandBuffer();
	if (!initCommandBuffer->Init(vulkanDevice, vulkanCommandPool, true))
	{
		printf("ERROR: Failed to create a command buffer! (initCommandBuffer)\n");
		return false;
	}
	
	if (!InitDepthBuffer())
	{
		printf("ERROR: Failed to init depth buffer!\n");
		return false;
	}
	
	VkAttachmentDescription attachmentDesc[2];
	attachmentDesc[0].format = vulkanDevice->GetFormat();
	attachmentDesc[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachmentDesc[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentDesc[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachmentDesc[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachmentDesc[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentDesc[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachmentDesc[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	attachmentDesc[0].flags = 0;
	
	attachmentDesc[1].format = depthImage.format;
	attachmentDesc[1].samples = VK_SAMPLE_COUNT_1_BIT;
	attachmentDesc[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentDesc[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachmentDesc[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
	attachmentDesc[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachmentDesc[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachmentDesc[1].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	attachmentDesc[1].flags = 0;
	
	VkAttachmentReference colorAttachmentRef;
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	
	VkAttachmentReference depthAttachmentRef;
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	
	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependency.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	
	VulkanRenderpassCI renderpassCI;
	renderpassCI.attachments = attachmentDesc;
	renderpassCI.attachmentCount = 2;
	renderpassCI.attachmentRefs = &colorAttachmentRef;
	renderpassCI.depthAttachmentRef = &depthAttachmentRef;
	renderpassCI.dependencies = &dependency;
	renderpassCI.dependenciesCount = 1;
	
	//forwardRenderPass = new VulkanRenderpass();
	//if (!forwardRenderPass->Init(vulkanDevice, &renderpassCI))
	//{
	//	gLogManager->AddMessage("ERROR: Failed to init main render pass!");
	//	return false;
	//}
	//
	//vulkanSwapchain = new VulkanSwapchain();
	//if (!vulkanSwapchain->Init(vulkanDevice, depthImage.view, forwardRenderPass))
	//{
	//	gLogManager->AddMessage("ERROR: Failed to create swapchain!");
	//	return false;
	//}
	//
	//if (!InitDeferredFramebuffer())
	//{
	//	gLogManager->AddMessage("ERROR: Failed to init deferred framebuffer!");
	//	return false;
	//}
	//
	//if (!InitColorSampler())
	//{
	//	gLogManager->AddMessage("ERROR: Failed to init color sampler!");
	//	return false;
	//}
	//
	//// Semaphores
	//VkSemaphoreCreateInfo semaphoreCI{};
	//semaphoreCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	//vkCreateSemaphore(vulkanDevice->GetDevice(), &semaphoreCI, VK_NULL_HANDLE, &imageReadySemaphore);
	//vkCreateSemaphore(vulkanDevice->GetDevice(), &semaphoreCI, VK_NULL_HANDLE, &drawCompleteSemaphore);
	//
	//// Pipeline cache
	//VkPipelineCacheCreateInfo pipelineCacheCI{};
	//pipelineCacheCI.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	//pipelineCacheCI.pNext = NULL;
	//VkResult result = vkCreatePipelineCache(vulkanDevice->GetDevice(), &pipelineCacheCI, VK_NULL_HANDLE, &pipelineCache);
	//if (result != VK_SUCCESS)
	//	return false;

	return true;
}
