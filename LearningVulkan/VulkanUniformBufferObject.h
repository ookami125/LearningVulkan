#pragma once
#include <vulkan\vulkan.h>

class VulkanDevice;

class VulkanUniformBufferObject
{
	VulkanDevice* device;
	
	void* uboData;
	size_t uboSize;

	VkBuffer uniformBuffer;
	VkDeviceMemory uniformBufferMemory;

	uint32_t count;

	bool dynamic;
public:
	VulkanUniformBufferObject(VulkanDevice* device, VkPhysicalDevice* physicalDevice, size_t uboSize, void* ubo, size_t count = 1, bool dynamic = false);
	~VulkanUniformBufferObject();

	void Update();
	VkBuffer GetBuffer();

	void* GetUBO();
	size_t GetBufferSize();

	size_t GetUBOSize();

	bool isDynamic();
};