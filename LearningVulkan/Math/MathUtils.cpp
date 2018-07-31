#include "MathUtils.h"

Mat4f MathUtils::projection(float angleOfView, float imageAspectRatio, float near_plane, float far_plane)
{
	float scale = tan(angleOfView * 0.5f * M_PI / 180) * near_plane;
	float r = imageAspectRatio * scale, l = -r;
	float t = scale, b = -t;
	return Mat4f(
		Vec4f(2 * near_plane / (r - l), 0, 0, 0),
		Vec4f(0, 2 * near_plane / (t - b), 0, 0),
		Vec4f((r + l) / (r - l), (t + b) / (t - b), -(far_plane + near_plane) / (far_plane - near_plane), -1),
		Vec4f(0, 0, -2 * far_plane * near_plane / (far_plane - near_plane), 0)
	);
}

Mat4f MathUtils::translate(float x, float y, float z)
{
	return Mat4f(
		Vec4f(1, 0, 0, x),
		Vec4f(0, 1, 0, y),
		Vec4f(0, 0, 1, z),
		Vec4f(0, 0, 0, 1)
	);
}

Mat4f MathUtils::translate(Vec4f v)
{
	float d[4];
	v.Store(d);
	return Mat4f(
		Vec4f(1, 0, 0, d[0]),
		Vec4f(0, 1, 0, d[1]),
		Vec4f(0, 0, 1, d[2]),
		Vec4f(0, 0, 0, 1)
	);
}

Mat4f MathUtils::scale(float x, float y, float z)
{
	return Mat4f(
		Vec4f(x, 0, 0, 0),
		Vec4f(0, y, 0, 0),
		Vec4f(0, 0, z, 0),
		Vec4f(0, 0, 0, 1)
	);
}

Mat4f MathUtils::scale(Vec4f v)
{
	float d[4];
	v.Store(d);
	return Mat4f(
		Vec4f(d[0], 0, 0, 0),
		Vec4f(0, d[1], 0, 0),
		Vec4f(0, 0, d[2], 0),
		Vec4f(0, 0, 0, 1)
	);
}

Mat4f MathUtils::rotation(const Quatf& quat)
{
	float s, x, y, z;
	quat.Store(&s, &x, &y, &z);
	const float x2 = x * x, y2 = y * y, z2 = z * z;
	const float sx = s * x, sy = s * y, sz = s * z;
	const float xz = x * z, yz = y * z, xy = x * y;
	return Mat4f(
		Vec4f(1 - 2 * (y2 + z2), 2 * (xy + sz), 2 * (xz - sy), 0.0f),
		Vec4f(2 * (xy - sz), 1 - 2 * (x2 + z2), 2 * (sx + yz), 0.0f),
		Vec4f(2 * (sy + xz), 2 * (yz - sx), 1 - 2 * (x2 + y2), 0.0f),
		Vec4f(0.0f, 0.0f, 0.0f, 1.0f));
}