#pragma once
#include <glm\glm.hpp>
#include <glm\gtc\quaternion.hpp>
#include <assimp\types.h>

glm::fquat fquat(aiQuaternion other);
glm::vec4 vec4(aiColor4D other);
glm::vec3 vec3(aiColor3D other);
glm::vec3 vec3(aiVector3D other);
glm::vec2 vec2(aiVector2D other);