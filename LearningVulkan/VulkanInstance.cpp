#include "VulkanInstance.h"
#include "VulkanException.h"
#include <vector>

VulkanInstance::VulkanInstance(std::vector<const char*> desiredExtensions, std::vector<const char*> desiredLayers)
{
	std::vector<const char*> availableDesiredExtensions;
	std::vector<VkExtensionProperties> availableExtensions = GetExtentionProperties();
	for (auto extenstion : desiredExtensions)
	{
		for (auto availableExtension : availableExtensions)
		{
			if (strcmp(availableExtension.extensionName, extenstion) == 0)
			{
				availableDesiredExtensions.push_back(extenstion);
				break;
			}
		}
	}

	printf("Available Extensions:\n");
	for (auto extension : availableExtensions)
		printf("  %s\n", extension.extensionName);

	std::vector<const char*> availableDesiredLayers;
	std::vector<VkLayerProperties> availableLayers = GetLayerProperties();
	for (auto layer : desiredLayers)
	{
		for (auto availableLayer : availableLayers)
		{
			if (strcmp(availableLayer.layerName, layer) == 0)
			{
				availableDesiredLayers.push_back(layer);
				break;
			}
		}
	}

	printf("Available Layers:\n");
	for (auto extension : availableLayers)
		printf("  %s\n", extension.layerName);

	VkApplicationInfo applicationInfo{};
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.pNext = nullptr;
	applicationInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
	applicationInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
	applicationInfo.pApplicationName = "";
	applicationInfo.pEngineName = "Cieric's Game Engine";

	VkInstanceCreateInfo instanceCreateInfo{};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pNext = nullptr;
	instanceCreateInfo.flags = 0;
	instanceCreateInfo.pApplicationInfo = &applicationInfo;
	instanceCreateInfo.enabledExtensionCount = availableDesiredExtensions.size();
	instanceCreateInfo.ppEnabledExtensionNames = availableDesiredExtensions.data();
	instanceCreateInfo.enabledLayerCount = availableDesiredLayers.size();
	instanceCreateInfo.ppEnabledLayerNames = availableDesiredLayers.data();

	if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance) != VK_SUCCESS)
		throw new VulkanException("Failed to create VkInstance!", __LINE__, __FILE__);
}

VulkanInstance::~VulkanInstance()
{
	vkDestroyInstance(instance, nullptr);
}

std::vector<VkExtensionProperties> VulkanInstance::GetExtentionProperties()
{
	uint32_t propertyCount;
	vkEnumerateInstanceExtensionProperties(nullptr, &propertyCount, nullptr);
	std::vector<VkExtensionProperties> properties(propertyCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &propertyCount, properties.data());
	return properties;

}

std::vector<VkLayerProperties> VulkanInstance::GetLayerProperties()
{
	uint32_t propertyCount;
	vkEnumerateInstanceLayerProperties(&propertyCount, nullptr);
	std::vector<VkLayerProperties> properties(propertyCount);
	vkEnumerateInstanceLayerProperties(&propertyCount, properties.data());
	return properties;

}
