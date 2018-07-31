#include "VulkanImageSampler.h"
#include "VulkanDevice.h"
#include "Logger.h"

VulkanImageSampler::VulkanImageSampler(VulkanDevice* device) : device(device)
{
	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = 16;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;
	if (vkCreateSampler(*device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
		LOGGER->Log("%s : %d", __FILE__, __LINE__);
		throw std::runtime_error("failed to create texture sampler!");
	}
}

VulkanImageSampler::~VulkanImageSampler()
{
	vkDestroySampler(*device, sampler, nullptr);
}

VulkanImageSampler::operator VkSampler() const
{
	return sampler;
}
