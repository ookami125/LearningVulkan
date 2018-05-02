#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan\vulkan.h>

class VulkanInstance;
class VulkanDevice;

class VulkanRenderer
{
	VulkanInstance* instance;
	VulkanDevice* device;
	VkSurfaceKHR surface;
public:
	VulkanRenderer(HWND hwnd);
	~VulkanRenderer();
};