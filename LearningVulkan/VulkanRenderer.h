#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan\vulkan.h>

class VulkanInstance;
class VulkanDevice;
class VulkanSwapchain;
class VulkanShader;
class VulkanShaderStage;
class VulkanPipeline;
class VulkanRenderPass;
class VulkanCommandPool;
struct Model;
struct Mesh;

class VulkanRenderer
{
	uint32_t width;
	uint32_t height;

	VulkanInstance* instance;
	VkPhysicalDevice physicalDevice;
	VulkanDevice* device;
	VulkanSwapchain* swapchain;
	VulkanShaderStage* shaderStage;
	VulkanPipeline* pipeline;
	VulkanRenderPass* renderPass;
	VulkanCommandPool* commandPool;

	VkCommandBuffer* activeCommandBuffer;

	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
public:
	VulkanRenderer(HWND hwnd);
	~VulkanRenderer();

	void UnregisterMesh(Mesh * mesh);
	void RegisterMesh(Mesh * mesh);
	void UnregisterModel(Model * model);
	void RegisterModel(Model * model);
	void RenderMesh(Mesh * mesh);
	void RenderModel(Model * model);
	void Render();
};