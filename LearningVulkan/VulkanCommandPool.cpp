#include "VulkanCommandPool.h"
#include "VulkanInstance.h"
#include "VulkanDevice.h"
#include "VulkanException.h"
#include <vulkan\vulkan.h>

VulkanCommandPool::VulkanCommandPool(VkPhysicalDevice* physicalDevice, VulkanDevice* device, VulkanInstance* instance) : device(device)
{
	QueueFamilyIndices queueFamilyIndices = instance->FindQueueFamilies(physicalDevice);

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if (vkCreateCommandPool(*device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
		throw new VulkanException("failed to create command pool!", __LINE__, __FILE__);
	}
}

VulkanCommandPool::~VulkanCommandPool()
{
	vkDestroyCommandPool(*device, commandPool, nullptr);
}

VulkanCommandPool::operator VkCommandPool() const
{
	return commandPool;
}
