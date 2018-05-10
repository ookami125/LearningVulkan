#include "VulkanUniformBufferObject.h"
#include "VulkanDevice.h"
#include "VulkanUtils.h"

VulkanUniformBufferObject::VulkanUniformBufferObject(VulkanDevice * device, VkPhysicalDevice* physicalDevice, size_t uboSize, void * ubo, bool dynamic) : device(device), uboData(ubo), uboSize(uboSize), dynamic(dynamic)
{
	uint32_t propertyFlags;
	propertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
	if (dynamic) propertyFlags |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	createBuffer(device, physicalDevice, uboSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, propertyFlags, uniformBuffer, uniformBufferMemory);
}

VulkanUniformBufferObject::~VulkanUniformBufferObject()
{
	vkDestroyBuffer(*device, uniformBuffer, nullptr);
	vkFreeMemory(*device, uniformBufferMemory, nullptr);
}

void VulkanUniformBufferObject::Update()
{
	if (dynamic)
	{
		void* data;
		vkMapMemory(*device, uniformBufferMemory, 0, uboSize, 0, &data);
		memcpy(data, uboData, uboSize);
		vkUnmapMemory(*device, uniformBufferMemory);
	}
	else
	{
		VkMappedMemoryRange memoryRange = {};
		memoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		memoryRange.memory = uniformBufferMemory;
		memoryRange.size = VK_WHOLE_SIZE;
		vkFlushMappedMemoryRanges(*device, 1, &memoryRange);
	}
}

VkBuffer VulkanUniformBufferObject::GetBuffer()
{
	return uniformBuffer;
}

size_t VulkanUniformBufferObject::GetUBOSize()
{
	return uboSize;
}

bool VulkanUniformBufferObject::isDynamic()
{
	return dynamic;
}

void * VulkanUniformBufferObject::GetUBO()
{
	return uboData;
}