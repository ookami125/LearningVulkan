#include "Vertex.h"

VkVertexInputBindingDescription Vertex::getBindingDescription()
{
	VkVertexInputBindingDescription bindingDescription = {};
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Vertex);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 5> Vertex::getAttributeDescriptions()
{
	std::array<VkVertexInputAttributeDescription, 5> attributeDescriptions = {};

	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(Vertex, pos);

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(Vertex, color);

	attributeDescriptions[2].binding = 0;
	attributeDescriptions[2].location = 2;
	attributeDescriptions[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

	attributeDescriptions[3].binding = 0;
	attributeDescriptions[3].location = 3;
	attributeDescriptions[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attributeDescriptions[3].offset = offsetof(Vertex, bonesIdx);

	attributeDescriptions[4].binding = 0;
	attributeDescriptions[4].location = 4;
	attributeDescriptions[4].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attributeDescriptions[4].offset = offsetof(Vertex, bonesWeights);

	return attributeDescriptions;
}

bool Vertex::operator==(const Vertex & other) const
{
	return pos == other.pos && color == other.color && texCoord == other.texCoord && bonesIdx == other.bonesIdx && bonesWeights == other.bonesWeights;
}

void hash_combine(size_t &seed, size_t hash)
{
	hash += 0x9e3779b9 + (seed << 6) + (seed >> 2);
	seed ^= hash;
}

namespace std {
	template<> struct hash<Vec4f> {
		size_t operator()(Vec4f const& vec) const {

			size_t seed = 0;
			float data[4]; vec.Store(data);
			hash_combine(seed, hash<float>()(data[0]));
			hash_combine(seed, hash<float>()(data[1]));
			hash_combine(seed, hash<float>()(data[2]));
			hash_combine(seed, hash<float>()(data[3]));
			return seed;
		}
	};

	template<> struct hash<Vertex> {
		size_t operator()(Vertex const& vertex) const {
			size_t seed = 0;
			hash_combine(seed, hash<Vec4f>()(vertex.pos));
			hash_combine(seed, hash<Vec4f>()(vertex.color));
			hash_combine(seed, hash<Vec4f>()(vertex.texCoord));
			return seed;
		}
	};
}