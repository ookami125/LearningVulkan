#pragma once
#include <string>
#include <assimp/texture.h>

class Texture;

class ResourceManager
{
public:
	static Texture* LoadTexture(std::string filepath);
	static Texture* LoadTexture(aiTexture* tex);
	static void CleanupTexture(Texture* tex);
};