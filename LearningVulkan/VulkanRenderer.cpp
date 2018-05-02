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

bool VulkanRenderer::InitColorSampler()
{
	VkResult result;

	VkSamplerCreateInfo samplerCI{};
	samplerCI.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCI.magFilter = VK_FILTER_LINEAR;
	samplerCI.minFilter = VK_FILTER_LINEAR;
	samplerCI.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerCI.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCI.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCI.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCI.mipLodBias = 0.0f;
	samplerCI.compareOp = VK_COMPARE_OP_NEVER;
	samplerCI.minLod = 0.0f;
	samplerCI.maxLod = 8.0f;
	samplerCI.maxAnisotropy = vulkanDevice->GetGPUProperties().limits.maxSamplerAnisotropy;
	samplerCI.anisotropyEnable = VK_TRUE;
	samplerCI.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	result = vkCreateSampler(vulkanDevice->GetDevice(), &samplerCI, VK_NULL_HANDLE, &colorSampler);
	if (result != VK_SUCCESS)
		return false;

	return true;
}

bool VulkanRenderer::InitDeferredFramebuffer()
{
	VkResult result;

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	positionAtt = new FrameBufferAttachment();
	if (!positionAtt->Create(vulkanDevice, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, initCommandBuffer,
		(uint32_t)width, (uint32_t)height, 1))
	{
		printf("ERROR: Failed to create position framebuffer attachment!\n");
		return false;
	}

	normalAtt = new FrameBufferAttachment();
	if (!normalAtt->Create(vulkanDevice, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, initCommandBuffer,
		(uint32_t)width, (uint32_t)height, 1))
	{
		printf("ERROR: Failed to create normal framebuffer attachment!\n");
		return false;
	}

	albedoAtt = new FrameBufferAttachment();
	if (!albedoAtt->Create(vulkanDevice, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, initCommandBuffer,
		(uint32_t)width, (uint32_t)height, 1))
	{
		printf("ERROR: Failed to create albedo framebuffer attachment!\n");
		return false;
	}

	materialAtt = new FrameBufferAttachment();
	if (!materialAtt->Create(vulkanDevice, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, initCommandBuffer,
		(uint32_t)width, (uint32_t)height, 1))
	{
		printf("ERROR: Failed to create material framebuffer attachment!\n");
		return false;
	}

	depthAtt = new FrameBufferAttachment();
	if (!depthAtt->Create(vulkanDevice, depthImage.format, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, initCommandBuffer,
		(uint32_t)width, (uint32_t)height, 1))
	{
		printf("ERROR: Failed to create depth framebuffer attachment!\n");
		return false;
	}

	std::vector<VkAttachmentDescription> attachmentDescs;
	std::vector<VkAttachmentReference> attachmentRefs;
	attachmentDescs.resize(5);
	attachmentRefs.resize(4);

	for (unsigned int i = 0; i < attachmentDescs.size(); i++)
	{
		attachmentDescs[i] = {};
		attachmentDescs[i].samples = VK_SAMPLE_COUNT_1_BIT;
		attachmentDescs[i].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentDescs[i].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachmentDescs[i].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDescs[i].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachmentDescs[i].flags = 0;
		attachmentDescs[i].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachmentDescs[i].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	}

	// Overwrite layout for depth
	attachmentDescs[4].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	attachmentDescs[4].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	attachmentDescs[0].format = positionAtt->GetFormat();
	attachmentDescs[1].format = normalAtt->GetFormat();
	attachmentDescs[2].format = albedoAtt->GetFormat();
	attachmentDescs[3].format = materialAtt->GetFormat();
	attachmentDescs[4].format = depthAtt->GetFormat();

	for (unsigned int i = 0; i < attachmentRefs.size(); i++)
	{
		attachmentRefs[i].attachment = i;
		attachmentRefs[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	}

	// Overwrite reference for depth
	VkAttachmentReference depthAttachmentRef;
	depthAttachmentRef.attachment = (uint32_t)attachmentDescs.size() - 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VulkanRenderpassCI renderpassCI;
	renderpassCI.attachments = (VkAttachmentDescription*)attachmentDescs.data();
	renderpassCI.attachmentCount = 5;
	renderpassCI.attachmentRefs = (VkAttachmentReference*)attachmentRefs.data();
	renderpassCI.depthAttachmentRef = &depthAttachmentRef;
	renderpassCI.dependencies = VK_NULL_HANDLE;
	renderpassCI.dependenciesCount = 0;

	deferredRenderPass = new VulkanRenderpass();
	if (!deferredRenderPass->Init(vulkanDevice, &renderpassCI))
	{
		printf("ERROR: Failed to init deferred renderpass!");
		return false;
	}

	std::vector<VkImageView> viewAttachments;
	viewAttachments.resize(5);

	viewAttachments[0] = *positionAtt->GetImageView();
	viewAttachments[1] = *normalAtt->GetImageView();
	viewAttachments[2] = *albedoAtt->GetImageView();
	viewAttachments[3] = *materialAtt->GetImageView();
	viewAttachments[4] = *depthAtt->GetImageView();

	VkFramebufferCreateInfo fbCI{};
	fbCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	fbCI.renderPass = deferredRenderPass->GetRenderpass();
	fbCI.pAttachments = viewAttachments.data();
	fbCI.attachmentCount = (uint32_t)viewAttachments.size();
	fbCI.width = width;
	fbCI.height = height;
	fbCI.layers = 1;

	result = vkCreateFramebuffer(vulkanDevice->GetDevice(), &fbCI, VK_NULL_HANDLE, &deferredFramebuffer);
	if (result != VK_SUCCESS)
		return false;

	attachmentsPtr.push_back(positionAtt);
	attachmentsPtr.push_back(normalAtt);
	attachmentsPtr.push_back(albedoAtt);
	attachmentsPtr.push_back(materialAtt);

	return true;
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t srcObj, size_t location,
	int32_t msgCode, const char * layer_prefix, const char * msg, void * userData)
{
	std::string outputMsg;
	if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) outputMsg = "[WARNING] ";
	else if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) outputMsg = "[PERF WARNING] ";
	else if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) outputMsg = "[ERROR] ";

	outputMsg += msg;
	printf("%s\n", outputMsg.c_str());

	return false;
}

bool VulkanRenderer::InitVulkanDebugMode()
{
	fvkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(*vulkanInstance, "vkCreateDebugReportCallbackEXT");
	fvkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(*vulkanInstance, "vkDestroyDebugReportCallbackEXT");
	if (fvkCreateDebugReportCallbackEXT == nullptr || fvkDestroyDebugReportCallbackEXT == nullptr)
	{
		printf("ERROR: Couldn't fetch one or more debug functions!\n");
		return false;
	}

	VkDebugReportCallbackCreateInfoEXT debugCI{};
	debugCI.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;

	debugCI.flags = VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT;
	debugCI.pfnCallback = VulkanDebugCallback;

	fvkCreateDebugReportCallbackEXT(*vulkanInstance, &debugCI, VK_NULL_HANDLE, &debugReport);
	return true;
}

void VulkanRenderer::UnloadVulkanDebugMode()
{
	fvkDestroyDebugReportCallbackEXT(*vulkanInstance, debugReport, VK_NULL_HANDLE);
	debugReport = VK_NULL_HANDLE;
}

VulkanRenderer::VulkanRenderer()
{
	vulkanInstance = NULL;
	vulkanDevice = NULL;
	vulkanCommandPool = NULL;
	initCommandBuffer = NULL;
	vulkanSwapchain = NULL;
	forwardRenderPass = NULL;
	deferredRenderPass = NULL;
	
	positionAtt = NULL;
	normalAtt = NULL;
	albedoAtt = NULL;
	materialAtt = NULL;
	depthAtt = NULL;
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
	
	forwardRenderPass = new VulkanRenderpass();
	if (!forwardRenderPass->Init(vulkanDevice, &renderpassCI))
	{
		printf("ERROR: Failed to init main render pass!/n");
		return false;
	}
	
	vulkanSwapchain = new VulkanSwapchain();
	if (!vulkanSwapchain->Init(window, vulkanDevice, depthImage.view, forwardRenderPass))
	{
		printf("ERROR: Failed to create swapchain!/n");
		return false;
	}

	if (!InitDeferredFramebuffer())
	{
		printf("ERROR: Failed to init deferred framebuffer!/n");
		return false;
	}
	
	if (!InitColorSampler())
	{
		printf("ERROR: Failed to init color sampler!/n");
		return false;
	}
	
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
