#include "VulkanRenderer.h"
#include "VulkanInstance.h"
#include "VulkanDevice.h"
#include "VulkanException.h"
#include <vector>

VulkanRenderer::VulkanRenderer(HWND hwnd)
{
	instance = new VulkanInstance({ "VK_KHR_surface", VK_EXT_DEBUG_REPORT_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME }, { "VK_LAYER_LUNARG_standard_validation" });
	instance->CreateSurface(hwnd);
	VkPhysicalDevice physicalDevice = instance->GetPhysicalSuitableDevice();
	device = new VulkanDevice(&physicalDevice, instance->GetSurface(), {}, { "VK_LAYER_LUNARG_standard_validation" });
}

VulkanRenderer::~VulkanRenderer()
{
	delete device;
	delete instance;
}


