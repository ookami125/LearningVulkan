#pragma once
#include <vulkan\vulkan.h>

class VulkanDevice;

class VulkanDynamicUBO
{
	VulkanDevice* device;

	void* uboData;
	size_t uboSize;
	uint32_t alignment;

	VkBuffer uniformBuffer;
	VkDeviceMemory uniformBufferMemory;

	uint32_t count;
public:
	VulkanDynamicUBO(VulkanDevice * device, VkPhysicalDevice * physicalDevice, size_t uboSize, size_t count);
	~VulkanDynamicUBO();

	void Write(int id, void * data);

	void Update(int id);
	VkBuffer GetBuffer();

	//void* GetUBO();
	size_t GetUBOCount();
	size_t GetAlignmentSize();
	size_t GetBufferSize();

	size_t GetUBOSize();
	void * GetUBO(uint32_t id);
};