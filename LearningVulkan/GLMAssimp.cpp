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

Vec4f vec3(aiVector3D other)
{
	return Vec4f(other.x, other.y, other.z, NAN);
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
		Vec4f(other.a1, other.a2, other.a3, other.a4),
		Vec4f(other.b1, other.b2, other.b3, other.b4),
		Vec4f(other.c1, other.c2, other.c3, other.c4),
		Vec4f(other.d1, other.d2, other.d3, other.d4)
	);
}

Quatf fquat(aiQuaternion other)
{
	return Quatf(other.w, other.x, other.y, other.z);
}
