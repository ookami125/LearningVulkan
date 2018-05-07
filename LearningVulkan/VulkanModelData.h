#pragma once
#include <vulkan\vulkan.h>

struct VulkanMeshData
{
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;
};

struct VulkanModelData
{
	VkDescriptorSet descriptorSet;
	VulkanUniformBufferObject* ubo;
};