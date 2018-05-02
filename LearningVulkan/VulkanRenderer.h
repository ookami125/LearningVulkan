#pragma once

#define VK_USE_PLATFORM_WIN32_KHR

#include <GLFW\glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vulkan\vulkan.h>

class VulkanInstance;

class VulkanRenderer
{
	VulkanInstance* instance;
public:
	VulkanRenderer();
	~VulkanRenderer();
};