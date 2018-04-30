#include "ResourceManager.h"
#include <vulkan\vulkan.h>
#include <stb_image.h>
#include <algorithm>

#include "Texture.h"

Texture* ResourceManager::LoadTexture(std::string filepath)
{
	throw "Function not yet implemented!";
	return nullptr;
}

Texture* ResourceManager::LoadTexture(aiTexture * tex)
{
	return new Texture(tex);
}

void ResourceManager::CleanupTexture(Texture * tex)
{
	delete tex;
}
