#pragma once
#include <vulkan\vulkan.h>
#include "Math/vec4.h"
#include <array>

struct Vertex {
	Vec4f pos;
	Vec4f color;
	Vec4f texCoord;
	uint32_t bonesIdx[4];
	Vec4f bonesWeights;

	static VkVertexInputBindingDescription getBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 5> getAttributeDescriptions();
	bool operator==(const Vertex& other) const;
};