#include "GLMAssimp.h"

Vec4f vec4(aiColor4D other)
{
	return Vec4f(other.r, other.g, other.b, other.a);
}

Vec4f vec3(aiColor4D other)
{
	return Vec4f(other.r, other.g, other.b, NAN);
}

Vec4f vec3(aiColor3D other)
{
	return Vec4f(other.r, other.g, other.b, NAN);
}

Vec4f vec3(aiVector3D other, float w)
{
	return Vec4f(other.x, other.y, other.z, w);
}

Vec4f vec2(aiVector2D other)
{
	return Vec4f(other.x, other.y, NAN, NAN);
}

Vec4f vec2(aiVector3D other)
{
	return Vec4f(other.x, other.y, NAN, NAN);
}

Mat4f mat4(aiMatrix4x4 other)
{
	return Mat4f(
		Vec4f(other.a1, other.b1, other.c1, other.d1),
		Vec4f(other.a2, other.b2, other.c2, other.d2),
		Vec4f(other.a3, other.b3, other.c3, other.d3),
		Vec4f(other.a4, other.b4, other.c4, other.d4)
	);
}

Quatf fquat(aiQuaternion other)
{
	return Quatf(other.w, other.x, other.y, other.z);
}
