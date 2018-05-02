#include "VulkanRenderer.h"
#include "VulkanInstance.h"
#include <vector>

VulkanRenderer::VulkanRenderer()
{
	instance = new VulkanInstance({}, { "VK_LAYER_LUNARG_standard_validation" });
}

VulkanRenderer::~VulkanRenderer()
{
	delete instance;
}


