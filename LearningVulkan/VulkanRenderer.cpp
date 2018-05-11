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
#include "VulkanModelData.h"
#include "VulkanTextureData.h"
#include "VulkanUtils.h"
#include "VulkanDescriptorPool.h"
#include "VulkanDescriptorSetLayout.h"
#include "VulkanUniformBufferObject.h"
#include "VulkanImageSampler.h"
#include "VulkanImage.h"
#include "Model.h"
#include "Vertex.h"
#include "Texture.h"
#include "Animation.h"
#include "Utils.h"
#include <vector>
#include <chrono>

VulkanRenderer::VulkanRenderer(HWND hwnd)
{
	RECT rect;
	GetClientRect(hwnd, &rect);
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;
	instance = new VulkanInstance(hwnd, { "VK_KHR_surface", VK_EXT_DEBUG_REPORT_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME }, { "VK_LAYER_LUNARG_standard_validation" });
	physicalDevice = instance->GetPhysicalSuitableDevice({ VK_KHR_SWAPCHAIN_EXTENSION_NAME  });
	device = new VulkanDevice(&physicalDevice, instance, { VK_KHR_SWAPCHAIN_EXTENSION_NAME }, { "VK_LAYER_LUNARG_standard_validation" });
	swapchain = new VulkanSwapchain(&physicalDevice, instance, device, width, height);
	shaderStage = new VulkanShaderStage(device, { "shaders/shader_base.vert.spv", "shaders/shader_base.frag.spv" });
	renderPass = new VulkanRenderPass(&physicalDevice, device, swapchain);
	descriptorSetLayout = new VulkanDescriptorSetLayout(device,
	{
		{
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			VK_SHADER_STAGE_VERTEX_BIT
		},
		{
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
			VK_SHADER_STAGE_VERTEX_BIT
		},
		{
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			VK_SHADER_STAGE_FRAGMENT_BIT
		}
	});

	pipeline = new VulkanPipeline(device, swapchain, shaderStage, renderPass, { descriptorSetLayout });

	uboViewProj = new UBOViewProj();
	vuboViewProj = new VulkanUniformBufferObject(device, &physicalDevice, sizeof(UBOViewProj), uboViewProj);
	
	dynamicAlignmentUBOModel = GetUBOAlignment(sizeof(UBOModel));
	size_t bufferSizeUBOModel = MAX_OBJECT_RENDER * dynamicAlignmentUBOModel;
	uboModel = (UBOModel*)alignedAlloc(bufferSizeUBOModel, dynamicAlignmentUBOModel);
	vuboModel = new VulkanUniformBufferObject(device, &physicalDevice, bufferSizeUBOModel, uboModel, true);

	//dynamicAlignmentUBOTextureIdx = GetUBOAlignment(sizeof(int));
	//size_t bufferSizeUBOTexture = MAX_OBJECT_RENDER * dynamicAlignmentUBOTextureIdx;
	//uboTextures = (int*)alignedAlloc(bufferSizeUBOTexture, dynamicAlignmentUBOTextureIdx);
	//vuboTextures = new VulkanUniformBufferObject(device, &physicalDevice, bufferSizeUBOTexture, uboTextures, true);

	descriptorPool = new VulkanDescriptorPool(device);

	descriptorSet = descriptorPool->AllocateDescriptorSet(descriptorSetLayout);
	descriptorPool->UpdateDescriptorSets(descriptorSet, 0, vuboViewProj);
	descriptorPool->UpdateDescriptorSets(descriptorSet, 1, vuboModel);
	//descriptorPool->UpdateDescriptorSets(descriptorSet, 3, vuboTextures);

	swapchain->InitFrameBuffers(renderPass);
	commandPool = new VulkanCommandPool(&physicalDevice, device, instance);
	swapchain->InitCommandBuffers(commandPool);

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	if (vkCreateSemaphore(*device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(*device, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS) {

		throw std::runtime_error("failed to create semaphores!");
	}
}

VulkanRenderer::~VulkanRenderer()
{
	vkDestroySemaphore(*device, renderFinishedSemaphore, nullptr);
	vkDestroySemaphore(*device, imageAvailableSemaphore, nullptr);
	delete commandPool;
	delete descriptorPool;
	delete vuboViewProj;
	delete uboViewProj;
	delete pipeline;
	delete descriptorSetLayout;
	delete renderPass;
	delete shaderStage;
	delete swapchain;
	delete device;
	delete instance;
}

void VulkanRenderer::UnregisterMesh(Mesh* mesh)
{
	vkDestroyBuffer(*device, ((VulkanMeshData*)mesh->rendererData)->indexBuffer, nullptr);
	vkFreeMemory(*device, ((VulkanMeshData*)mesh->rendererData)->indexBufferMemory, nullptr);
	vkDestroyBuffer(*device, ((VulkanMeshData*)mesh->rendererData)->vertexBuffer, nullptr);
	vkFreeMemory(*device, ((VulkanMeshData*)mesh->rendererData)->vertexBufferMemory, nullptr);
	free(mesh->rendererData);
}

void VulkanRenderer::RegisterMesh(Mesh* mesh)
{
	mesh->rendererData = malloc(sizeof(VulkanMeshData));

	VulkanMeshData& modelData = *((VulkanMeshData*)mesh->rendererData);
	{
		VkDeviceSize bufferSize = sizeof(mesh->vertices[0]) * mesh->vertices_count;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createBuffer(device, &physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(*device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, mesh->vertices, (size_t)bufferSize);
		vkUnmapMemory(*device, stagingBufferMemory);

		createBuffer(device, &physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, modelData.vertexBuffer, modelData.vertexBufferMemory);

		copyBuffer(commandPool, device, stagingBuffer, modelData.vertexBuffer, bufferSize);

		vkDestroyBuffer(*device, stagingBuffer, nullptr);
		vkFreeMemory(*device, stagingBufferMemory, nullptr);
	}
	{
		VkDeviceSize bufferSize = sizeof(mesh->indices[0]) * mesh->indices_count;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createBuffer(device, &physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(*device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, mesh->indices, (size_t)bufferSize);
		vkUnmapMemory(*device, stagingBufferMemory);

		createBuffer(device, &physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, modelData.indexBuffer, modelData.indexBufferMemory);

		copyBuffer(commandPool, device, stagingBuffer, modelData.indexBuffer, bufferSize);

		vkDestroyBuffer(*device, stagingBuffer, nullptr);
		vkFreeMemory(*device, stagingBufferMemory, nullptr);
	}
}

void VulkanRenderer::UnregisterModel(Model* model)
{
	//delete ((VulkanModelData*)model->rendererData)->vubo;
	//delete ((VulkanModelData*)model->rendererData)->ubo;
	free(model->rendererData);

	for (Mesh* mesh : model->meshes)
		UnregisterMesh(mesh);
	for (Texture* texture : model->textures)
		UnregisterTexture(texture);
}

void VulkanRenderer::RegisterModel(Model* model)
{
	for (Mesh* mesh : model->meshes)
		RegisterMesh(mesh);
	if (model->textures.size() == 0)
	{
		aiTexture* tex = new aiTexture();
		tex->mHeight = 2;
		tex->mWidth = 2;
		tex->pcData = (aiTexel*)malloc(sizeof(aiTexel) * 4);
		model->textures.push_back(new Texture(tex));
	}
	for (Texture* texture : model->textures)
		RegisterTexture(texture);

	model->rendererData = malloc(sizeof(VulkanModelData));

	//((VulkanModelData*)model->rendererData)->ubo = new UBOModel();
	//descriptorPool->UpdateDescriptorSets(descriptorSet, 1, ((VulkanModelData*)model->rendererData)->vubo);
	descriptorPool->UpdateDescriptorSets(descriptorSet, 2, model->textures[0]); //TOF
}

void VulkanRenderer::UnregisterTexture(Texture * texture)
{
	delete ((VulkanTextureData*)texture->rendererData)->image;
	delete ((VulkanTextureData*)texture->rendererData)->sampler;
	free(texture->rendererData);
}

void VulkanRenderer::RegisterTexture(Texture* texture)
{
	uint32_t imageSize = sizeof(texture->bytes[0]) * texture->width * texture->height * 4;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(device, &physicalDevice, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(*device, stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, texture->bytes, static_cast<size_t>(imageSize));
	vkUnmapMemory(*device, stagingBufferMemory);

	VulkanImage* image = new VulkanImage(
		device,
		&physicalDevice,
		texture->width,
		texture->height,
		VK_FORMAT_R8G8B8A8_UNORM,
		VK_IMAGE_ASPECT_COLOR_BIT,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);
	image->TransitionImageLayout(commandPool, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	image->CopyFromBuffer(stagingBuffer, texture->width, texture->height, device, commandPool);
	image->TransitionImageLayout(commandPool, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	vkDestroyBuffer(*device, stagingBuffer, nullptr);
	vkFreeMemory(*device, stagingBufferMemory, nullptr);

	texture->rendererData = malloc(sizeof(VulkanTextureData));
	((VulkanTextureData*)texture->rendererData)->image = image;
	((VulkanTextureData*)texture->rendererData)->sampler = new VulkanImageSampler(device);
}

void VulkanRenderer::RenderMesh(Mesh* mesh)
{
	vkCmdBindPipeline(activeCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipeline);
	uint32_t dynamicOffset = renderCount * static_cast<uint32_t>(dynamicAlignmentUBOModel);
	vkCmdBindDescriptorSets(activeCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetLayout(), 0, 1, &descriptorSet, 1, &dynamicOffset);
	VkBuffer vertexBuffers[] = { ((VulkanMeshData*)mesh->rendererData)->vertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(activeCommandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(activeCommandBuffer, ((VulkanMeshData*)mesh->rendererData)->indexBuffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdDrawIndexed(activeCommandBuffer, static_cast<uint32_t>(mesh->indices_count), 1, 0, 0, 0);
}

void VulkanRenderer::RenderModel(Model* model)
{
	static auto startTime = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
	auto animFrame = model->GetAnimationFrame(0, 0, time);
	memcpy(uboModel[renderCount].bones, animFrame.data(), sizeof(glm::mat4) * std::min((size_t)MAX_BONE_COUNT, animFrame.size()));
	uboModel[renderCount].model = glm::rotate(glm::rotate(glm::scale(glm::mat4(1.0f), glm::vec3(0.2f)), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	vuboModel->Update();

	for (Mesh* mesh : model->meshes)
		RenderMesh(mesh);
	//printf("renderCount: %d\n", renderCount);
	renderCount += 1;
}

void VulkanRenderer::StartRender()
{
	renderCount = 0;
	swapchain->NextFrame();

	uboViewProj->view = glm::lookAt(glm::vec3(10.0f, 10.0f, 10.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	uboViewProj->proj = glm::perspective(glm::radians(45.0f), width / (float)height, 0.1f, 100.0f);
	uboViewProj->proj[1][1] *= -1;
	vuboViewProj->Update();

	activeCommandBuffer = swapchain->GetNextCommandBuffer();
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	beginInfo.pInheritanceInfo = nullptr; // Optional

	if (vkBeginCommandBuffer(activeCommandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = *renderPass;
	renderPassInfo.framebuffer = swapchain->GetNextFrameBuffer();
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = swapchain->GetExtent();

	std::array<VkClearValue, 2> clearValues = {};
	clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	clearValues[1].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(activeCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(activeCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipeline);
}

void VulkanRenderer::EndRender()
{
	vkCmdEndRenderPass(activeCommandBuffer);

	if (vkEndCommandBuffer(activeCommandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}
}

void VulkanRenderer::Present()
{
	uint32_t imageIndex;
	vkAcquireNextImageKHR(*device, *swapchain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
	
	auto image = swapchain->GetImage(imageIndex);
	//image->TransitionImageLayout(commandPool, swapchain->GetFormat(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &activeCommandBuffer;

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(device->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	//
	image->TransitionImageLayout(commandPool, swapchain->GetFormat(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

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

