#include "Texture.h"
#include <stb_image.h>

#include <fstream>

Texture::Texture(aiTexture * tex)
{
	if (tex->mHeight > 0) {
		width = tex->mWidth;
		height = tex->mHeight;
		size_t size = sizeof(uint8_t) * width * height * 4;
		bytes = (uint8_t*)malloc(size);
		memcpy_s(bytes, size, tex->pcData, size);
		format = VK_FORMAT_B8G8R8A8_UNORM;
	} else {
		int texWidth, texHeight, texChannels;
		uint8_t* pixels = stbi_load_from_memory((stbi_uc*)tex->pcData, tex->mWidth, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		width = texWidth;
		height = texHeight;
		size_t size = sizeof(uint8_t) * width * height * 4;
		bytes = (uint8_t*)malloc(size);
		memcpy_s(bytes, size, pixels, size);
		format = VK_FORMAT_R8G8B8A8_UNORM;
	}
}

Texture::~Texture()
{
	free(bytes);
}
