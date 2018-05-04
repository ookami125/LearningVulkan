#include "VulkanUniformBufferObject.h"
#include "VulkanDevice.h"
#include "VulkanUtils.h"

VulkanUniformBufferObject::VulkanUniformBufferObject(VulkanDevice * device, VkPhysicalDevice* physicalDevice, size_t uboSize, void * ubo) : device(device), uboData(ubo), uboSize(uboSize)
{
	createBuffer(device, physicalDevice, uboSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffer, uniformBufferMemory);
}

VulkanUniformBufferObject::~VulkanUniformBufferObject()
{
	vkDestroyBuffer(*device, uniformBuffer, nullptr);
	vkFreeMemory(*device, uniformBufferMemory, nullptr);
}

void VulkanUniformBufferObject::Update()
{
	void* data;
	vkMapMemory(*device, uniformBufferMemory, 0, uboSize, 0, &data);
	memcpy(data, uboData, uboSize);
	vkUnmapMemory(*device, uniformBufferMemory);
}

VkBuffer VulkanUniformBufferObject::GetBuffer()
{
	return uniformBuffer;
}

size_t VulkanUniformBufferObject::GetBufferSize()
{
	return uboSize;
}
