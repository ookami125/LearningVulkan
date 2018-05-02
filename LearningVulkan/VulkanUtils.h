#pragma once
#include <vulkan\vulkan.h>
#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"

template<typename t>
inline void SafeDelete(t mem)
{
	if (!mem)
		return;
	delete mem;
	mem = nullptr;
}

template<typename t, typename x>
inline void SafeUnload(t mem, x p1)
{
	if (!mem)
		return;
	mem->Unload(p1);
	delete mem;
	mem = nullptr;
}

template<typename t, typename x, typename y>
inline void SafeUnload(t mem, x p1, y p2)
{
	if (!mem)
		return;
	mem->Unload(p1, p2);
	delete mem;
	mem = nullptr;
}

void SetImageLayout(VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout,
	VkImageSubresourceRange * range, VulkanCommandBuffer * cmdBuffer, VulkanDevice * device, bool beginAndExecCmdBuffer);