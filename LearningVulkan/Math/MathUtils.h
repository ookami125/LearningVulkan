#pragma once
#include "mat4.h"
#include "quat.h"

namespace MathUtils
{
	const static float M_PI = 3.141592653589f;

	Mat4f projection(float angleOfView, float imageAspectRatio, float near_plane, float far_plane);
	Mat4f translate(float x, float y, float z);
	Mat4f translate(Vec4f v);
	Mat4f scale(float x, float y, float z);
	Mat4f scale(Vec4f v);
	Mat4f rotation(const Quatf& quat);
}