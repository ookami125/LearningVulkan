#pragma once
#include <vulkan\vulkan.h>
#include "VulkanException.h"

class VulkanDevice;
class VulkanCommandPool;

uint32_t FindMemoryType(VkPhysicalDevice* physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

void createBuffer(VulkanDevice* device, VkPhysicalDevice* physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

void copyBuffer(VulkanCommandPool* commandPool, VulkanDevice* device, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);