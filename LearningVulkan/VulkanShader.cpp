#include "VulkanShader.h"
#include "VulkanDevice.h"
#include "VulkanException.h"
#include <vulkan\vulkan.h>
#include <fstream>
#include <vector>
#include "Logger.h"

VulkanShader::VulkanShader(VulkanDevice* device, std::string filepath) : device(device)
{
	std::ifstream file(filepath, std::ios::ate | std::ios::binary);
	if (!file.is_open())
	{
		LOGGER->Log("%s : %d", __FILE__, __LINE__);
		throw std::runtime_error("failed to open file!");
	}
	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = buffer.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());

	if (vkCreateShaderModule(*device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
	{
		LOGGER->Log("%s : %d", __FILE__, __LINE__);
		throw new VulkanException("failed to create shader module!", __LINE__, __FILE__);
	}
}

VulkanShader::~VulkanShader()
{
	vkDestroyShaderModule(*device, shaderModule, nullptr);
}

VkShaderModule VulkanShader::GetModule()
{
	return shaderModule;
}
