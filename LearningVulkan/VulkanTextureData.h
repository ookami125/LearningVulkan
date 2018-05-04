#pragma once
#include <vulkan\vulkan.h>

class VulkanImage;
class VulkanImageSampler;

struct VulkanTextureData
{
	VulkanImage* image;
	VulkanImageSampler* sampler;
};