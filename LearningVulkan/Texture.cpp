#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <fstream>

Texture::Texture(aiTexture * tex)
{
	if (tex->mHeight > 0) {
		width = tex->mWidth;
		height = tex->mHeight;
		size_t size = sizeof(uint8_t) * width * height * 4;
		auto indata = (uint8_t*)tex->pcData;
		bytes = (uint8_t*)malloc(size);
		for (uint32_t i = 0; i < size; i+=4)
		{
			bytes[i + 0] = indata[i + 2];
			bytes[i + 1] = indata[i + 1];
			bytes[i + 2] = indata[i + 0];
			bytes[i + 3] = indata[i + 3];
		}
	} else {
		int texWidth, texHeight, texChannels;
		uint8_t* pixels = stbi_load_from_memory((stbi_uc*)tex->pcData, tex->mWidth, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		width = texWidth;
		height = texHeight;
		size_t size = sizeof(uint8_t) * width * height * 4;
		bytes = (uint8_t*)malloc(size);
		memcpy_s(bytes, size, pixels, size);
	}
}

Texture::~Texture()
{
	free(bytes);
}
