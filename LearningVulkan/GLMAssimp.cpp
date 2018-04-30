#include "GLMAssimp.h"

glm::vec4 vec4(aiColor4D other)
{
	return glm::vec4(other.r, other.g, other.b, other.a);
}

glm::vec3 vec3(aiColor3D other)
{
	return glm::vec3(other.r, other.g, other.b);
}

glm::vec3 vec3(aiVector3D other)
{
	return glm::vec3(other.x, other.y, other.z);
}

glm::vec2 vec2(aiVector2D other)
{
	return glm::vec2(other.x, other.y);
}