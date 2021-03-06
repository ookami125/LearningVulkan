#pragma once
#include <string>
#include <assimp/texture.h>

struct Texture;

class ResourceManager
{
public:
	static Texture* LoadTexture(std::string filepath);
	static Texture* LoadTexture(aiTexture* tex);
	static void CleanupTexture(Texture* tex);
};