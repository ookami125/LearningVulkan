#pragma once
#include <vulkan\vulkan.h>
#include <vector>
#include <string>
class VulkanShader;
class VulkanDevice;

class VulkanShaderStage
{
	VulkanShader* vertexShader = nullptr;
	VulkanShader* geometryShader = nullptr;
	VulkanShader* fragmentShader = nullptr;

	std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
public:
	VulkanShaderStage(VulkanDevice* device, std::vector<std::string> files);
	~VulkanShaderStage();

	std::vector<VkPipelineShaderStageCreateInfo> GetStages();
};