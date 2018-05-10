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

	bool dynamic;
public:
	VulkanUniformBufferObject(VulkanDevice* device, VkPhysicalDevice* physicalDevice, size_t uboSize, void* ubo, bool dynamic = false);
	~VulkanUniformBufferObject();

	void Update();
	VkBuffer GetBuffer();

	void* GetUBO();
	size_t GetUBOSize();

	bool isDynamic();
};