#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan\vulkan.h>
#include "Math/mat4.h"
#include "Math/vec4.h"
#include <vector>

#define MAX_OBJECT_RENDER 5
#define MAX_BONE_COUNT 62

class VulkanInstance;
class VulkanDevice;
class VulkanSwapchain;
class VulkanShader;
class VulkanShaderStage;
class VulkanPipeline;
class VulkanRenderPass;
class VulkanCommandPool;
class VulkanUniformBufferObject;
class VulkanDynamicUBO;
class VulkanDescriptorSetLayout;
class VulkanDescriptorPool;
class VulkanImageSampler;
struct Model;
struct Mesh;
struct Texture;

struct alignas(32) UBOViewProj {
	Mat4f view;
	Mat4f proj;
};

struct alignas(32) UBOModel {
	//uint32_t diffuseTexture;
	Mat4f model;
	Mat4f bones[MAX_BONE_COUNT];
	uint32_t arrayIndex[4];
};

class VulkanRenderer
{
	uint32_t width;
	uint32_t height;

	uint32_t renderCount = 0;
	//uint32_t dynamicAlignmentUBOModel = 256;
	//uint32_t dynamicAlignmentUBOTextureIdx = 256;

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
	std::vector<VkDescriptorSet> descriptorSets;

	UBOViewProj* uboViewProj;
	VulkanUniformBufferObject* vuboViewProj;

	//UBOModel* uboModel;
	VulkanDynamicUBO* vuboModel;

	int* uboTextures;
	VulkanUniformBufferObject* vuboTextures;

	VulkanImageSampler* imageSmapler;

	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
public:

	float x = 10.0f, y = -5.0f, z = 10.0f;

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