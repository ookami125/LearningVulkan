#pragma once
#include <vulkan\vulkan.h>
#include <string>

class VulkanDevice;

class VulkanShader
{
	VulkanDevice* device;

	VkShaderModule shaderModule;

public:
	VulkanShader(VulkanDevice* device, std::string filepath);
	~VulkanShader();

	VkShaderModule GetModule();
};