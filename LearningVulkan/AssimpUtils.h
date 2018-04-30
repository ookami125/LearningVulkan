#pragma once
#include <assimp/scene.h>

std::string aiTextureTypeName(aiTextureType type)
{
	switch (type)
	{
	case aiTextureType_NONE:
		return "NONE";
	case aiTextureType_DIFFUSE:
		return "DIFFUSE";
	case aiTextureType_SPECULAR:
		return "SPECULAR";
	case aiTextureType_AMBIENT:
		return "AMBIENT";
	case aiTextureType_EMISSIVE:
		return "EMISSIVE";
	case aiTextureType_HEIGHT:
		return "HEIGHT";
	case aiTextureType_NORMALS:
		return "NORMALS";
	case aiTextureType_SHININESS:
		return "SHININESS";
	case aiTextureType_OPACITY:
		return "OPACITY";
	case aiTextureType_DISPLACEMENT:
		return "DISPLACEMENT";
	case aiTextureType_LIGHTMAP:
		return "LIGHTMAP";
	case aiTextureType_REFLECTION:
		return "REFLECTION";
	case aiTextureType_UNKNOWN:
		return "UNKNOWN";
	default:
		return "NONE";
	}
}