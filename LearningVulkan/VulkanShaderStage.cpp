#include "VulkanShaderStage.h"
#include <vulkan\vulkan.h>
#include "VulkanDevice.h"
#include "VulkanShader.h"

inline bool ends_with(std::string const & value, std::string const & ending)
{
	if (ending.size() > value.size()) return false;
	return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

VulkanShaderStage::VulkanShaderStage(VulkanDevice* device, std::vector<std::string> files)
{
	for (std::string file : files)
	{
		if (ends_with(file, ".vert.spv") && !vertexShader)
			vertexShader = new VulkanShader(device, file);
		if (ends_with(file, ".frag.spv") && !fragmentShader)
			fragmentShader = new VulkanShader(device, file);
		if (ends_with(file, ".geom.spv") && !geometryShader)
			geometryShader = new VulkanShader(device, file);
	}

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertexShader->GetModule();
	vertShaderStageInfo.pName = "main";
	shaderStages.push_back(vertShaderStageInfo);

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragmentShader->GetModule();
	fragShaderStageInfo.pName = "main";
	shaderStages.push_back(fragShaderStageInfo);
}

VulkanShaderStage::~VulkanShaderStage()
{
	delete vertexShader;
	delete fragmentShader;
	delete geometryShader;
}

std::vector<VkPipelineShaderStageCreateInfo> VulkanShaderStage::GetStages()
{
	return shaderStages;
}
