#pragma once
#include <vulkan\vulkan.h>
#include <vector>
#include "VulkanException.h"

class VulkanDevice;
class VulkanCommandPool;

uint32_t FindMemoryType(VkPhysicalDevice* physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

void createBuffer(VulkanDevice* device, VkPhysicalDevice* physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

void copyBuffer(VulkanCommandPool* commandPool, VulkanDevice* device, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

VkFormat FindSupportedFormat(VkPhysicalDevice* physicalDevice, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

VkFormat FindDepthFormat(VkPhysicalDevice* physicalDevice);

bool HasStencilComponent(VkFormat format);

VkCommandBuffer beginSingleTimeCommands(VulkanDevice* device, VulkanCommandPool* commandPool);

void endSingleTimeCommands(VkCommandBuffer commandBuffer, VulkanDevice * device, VulkanCommandPool * commandPool);

size_t GetUBOAlignment(size_t uboSize);