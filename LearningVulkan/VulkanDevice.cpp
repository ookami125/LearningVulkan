#include "VulkanDevice.h"
#include "VulkanException.h"
#include "VulkanInstance.h"

VulkanDevice::VulkanDevice(VkPhysicalDevice * physicalDevice, VulkanInstance* instance, std::vector<const char*> desiredExtensions, std::vector<const char*> desiredLayers)
{
	QueueFamilyIndices indices = instance->FindQueueFamilies(physicalDevice);

	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;
	queueCreateInfo.queueCount = 1;
	float queuePriority = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriority;


	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.queueCreateInfoCount = 1;

	VkPhysicalDeviceFeatures deviceFeatures = {};
	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = desiredExtensions.size();
	createInfo.ppEnabledExtensionNames = desiredExtensions.data();
	createInfo.enabledLayerCount = desiredLayers.size();
	createInfo.ppEnabledLayerNames = desiredLayers.data();

	if (vkCreateDevice(*physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
		throw new VulkanException("failed to create logical device!", __LINE__, __FILE__);
	}
	
	vkGetDeviceQueue(device, indices.graphicsFamily, 0, &deviceGraphicsQueue);
	vkGetDeviceQueue(device, indices.presentFamily, 0, &devicePresentQueue);
}

VulkanDevice::~VulkanDevice()
{
	vkDestroyDevice(device, nullptr);
}

VkQueue VulkanDevice::GetGraphicsQueue()
{
	return deviceGraphicsQueue;
}

VkQueue VulkanDevice::GetPresentQueue()
{
	return devicePresentQueue;
}

VulkanDevice::operator VkDevice() const
{
	return device;
}
