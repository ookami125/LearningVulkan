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
public:
	VulkanUniformBufferObject(VulkanDevice* device, VkPhysicalDevice* physicalDevice, size_t uboSize, void* ubo);
	~VulkanUniformBufferObject();

	void Update();
	VkBuffer GetBuffer();
	size_t GetBufferSize();
};