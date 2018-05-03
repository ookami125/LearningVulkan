#pragma once
#include <vulkan\vulkan.h>

struct VulkanModelData
{
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;
};