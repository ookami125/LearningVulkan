#pragma once
#include <vulkan\vulkan.h>
#include <glm\glm.hpp>
#include <array>

struct Vertex {
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;
	glm::uvec4 bonesIdx;
	glm::vec4 bonesWeights;

	static VkVertexInputBindingDescription getBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 5> getAttributeDescriptions();
	bool operator==(const Vertex& other) const;
};