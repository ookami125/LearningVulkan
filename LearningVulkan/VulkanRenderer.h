#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan\vulkan.h>

#define MAX_OBJECT_RENDER 5
#define MAX_BONE_COUNT 63

class VulkanInstance;
class VulkanDevice;
class VulkanSwapchain;
class VulkanShader;
class VulkanShaderStage;
class VulkanPipeline;
class VulkanRenderPass;
class VulkanCommandPool;
class VulkanUniformBufferObject;
class VulkanDescriptorSetLayout;
class VulkanDescriptorPool;
class VulkanImageSampler;
struct Model;
struct Mesh;
struct Texture;

struct UBOViewProj {
	glm::mat4 view;
	glm::mat4 proj;
};

struct UBOModel {
	glm::mat4 model;
	glm::mat4 bones[MAX_BONE_COUNT];
};

class VulkanRenderer
{
	uint32_t width;
	uint32_t height;

	uint32_t renderCount = 0;
	uint32_t dynamicAlignmentUBOModel = 256;
	uint32_t dynamicAlignmentUBOTextureIdx = 256;

	VulkanInstance* instance;
	VkPhysicalDevice physicalDevice;
	VulkanDevice* device;
	VulkanSwapchain* swapchain;
	VulkanShaderStage* shaderStage;
	VulkanPipeline* pipeline;
	VulkanRenderPass* renderPass;
	VulkanCommandPool* commandPool;

	VkCommandBuffer activeCommandBuffer;

	VulkanDescriptorSetLayout* descriptorSetLayout;

	VulkanDescriptorPool* descriptorPool;
	VkDescriptorSet descriptorSet;

	UBOViewProj* uboViewProj;
	VulkanUniformBufferObject* vuboViewProj;

	UBOModel* uboModel;
	VulkanUniformBufferObject* vuboModel;

	int* uboTextures;
	VulkanUniformBufferObject* vuboTextures;

	VulkanImageSampler* imageSmapler;

	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
public:
	VulkanRenderer(HWND hwnd);
	~VulkanRenderer();

	void UnregisterMesh(Mesh * mesh);
	void RegisterMesh(Mesh * mesh);

	void UnregisterModel(Model * model);
	void RegisterModel(Model * model);

	void UnregisterTexture(Texture * texture);
	void RegisterTexture(Texture * texture);

	void RenderMesh(Mesh * mesh);
	void RenderModel(Model * model);

	void StartRender();
	void EndRender();
	void Present();
};