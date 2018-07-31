#pragma once
//#include <glm\glm.hpp>
//#include <glm\gtc\quaternion.hpp>
#include "Math/vec4.h"
#include "Math/mat4.h"
#include "Math/quat.h"
#include <assimp\types.h>

Vec4f vec4(aiColor4D other);
Vec4f vec3(aiColor4D other);
Vec4f vec3(aiColor3D other);
Vec4f vec3(aiVector3D other, float w);
//Vec4f vec3(aiVector3D other);
Vec4f vec2(aiVector2D other);
Vec4f vec2(aiVector3D other);
Mat4f mat4(aiMatrix4x4 other);
Quatf fquat(aiQuaternion other);
//vec3 vec3(aiColor3D other);
//vec3 vec3(aiVector3D other);
//vec2 vec2(aiVector2D other);