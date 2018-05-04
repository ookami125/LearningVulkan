#pragma once
#include <vulkan\vulkan.h>
#include <stdint.h>
#include <assimp\texture.h>

struct Texture
{
	uint8_t* bytes;
	uint32_t width;
	uint32_t height;
	void* rendererData = nullptr;

	Texture(aiTexture* tex);
	~Texture();
};