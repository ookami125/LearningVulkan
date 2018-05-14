#include "VulkanDynamicUBO.h"
#include "VulkanDevice.h"
#include "VulkanUtils.h"
#include "Utils.h"

VulkanDynamicUBO::VulkanDynamicUBO(VulkanDevice * device, VkPhysicalDevice * physicalDevice, size_t uboSize, size_t count) : device(device), uboSize(uboSize), count(count)
{
	alignment = GetUBOAlignment(uboSize);
	size_t bufferSize = count * alignment;
	uboData = alignedAlloc(bufferSize, alignment);
	uint32_t propertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	createBuffer(device, physicalDevice, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, propertyFlags, uniformBuffer, uniformBufferMemory);
}

VulkanDynamicUBO::~VulkanDynamicUBO()
{
	vkDestroyBuffer(*device, uniformBuffer, nullptr);
	vkFreeMemory(*device, uniformBufferMemory, nullptr);
	alignedFree(uboData);
}

void VulkanDynamicUBO::Write(int id, void* data)
{
	memcpy((char*)uboData + (uboSize*id), data, uboSize);
}

void VulkanDynamicUBO::Update(int id)
{
	//VkMappedMemoryRange memoryRange = {};
	//memoryRange.pNext = nullptr;
	//memoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	//memoryRange.offset = 0;// uboSize * id;
	//memoryRange.memory = uniformBufferMemory;
	//memoryRange.size = GetBufferSize();
	//vkFlushMappedMemoryRanges(*device, 1, &memoryRange);
	void* data;
	vkMapMemory(*device, uniformBufferMemory, 0, GetBufferSize(), 0, &data);
	memcpy(data, uboData, GetBufferSize());
	vkUnmapMemory(*device, uniformBufferMemory);
}

VkBuffer VulkanDynamicUBO::GetBuffer()
{
	return uniformBuffer;
}

size_t VulkanDynamicUBO::GetBufferSize()
{
	return uboSize * count;
}

size_t VulkanDynamicUBO::GetUBOSize()
{
	return uboSize;
}

void * VulkanDynamicUBO::GetUBO(uint32_t id)
{
	return (void*)((uint8_t*)uboData + (id * alignment));
}

size_t VulkanDynamicUBO::GetUBOCount()
{
	return count;
}

size_t VulkanDynamicUBO::GetAlignmentSize()
{
	return alignment;
}