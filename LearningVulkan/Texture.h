#pragma once
#include <vulkan\vulkan.h>
#include <stdint.h>
#include <assimp\texture.h>
class Texture
{
public:
	uint8_t* bytes;
	uint32_t width;
	uint32_t height;
	VkFormat format;
	Texture(aiTexture* tex);
	~Texture();
};