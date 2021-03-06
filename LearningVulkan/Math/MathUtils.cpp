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

//Mat4f MathUtils::rotation(const Quatf& quat)
//{
//	float w, x, y, z;
//	quat.Store(&w, &x, &y, &z);
//	const float x2 = x * x, y2 = y * y, z2 = z * z;
//	const float wx = w * x, wy = w * y, wz = w * z;
//	const float xz = x * z, yz = y * z, xy = x * y;
//	return Mat4f(
//		Vec4f(1 - 2 * (y2 + z2), 0 + 2 * (xy + wz), 0 + 2 * (xz - wy), 0.0f),
//		Vec4f(0 + 2 * (xy - wz), 1 - 2 * (x2 + z2), 0 + 2 * (wx + yz), 0.0f),
//		Vec4f(0 + 2 * (wy + xz), 0 + 2 * (yz - wx), 1 - 2 * (x2 + y2), 0.0f),
//		Vec4f(             0.0f,              0.0f,              0.0f, 1.0f));
//}

Mat4f MathUtils::rotation(const Quatf& quat)
{
	float w, x, y, z;
	quat.Store(&w, &x, &y, &z);
	float qxx(x * x);
	float qyy(y * y);
	float qzz(z * z);
	float qxz(x * z);
	float qxy(x * y);
	float qyz(y * z);
	float qwx(w * x);
	float qwy(w * y);
	float qwz(w * z);

	Mat4f Result(
		Vec4f(1.0f - 2.0f * (qyy + qzz), 2.0f * (qxy - qwz), 2.0f * (qxz + qwy), 0.0f),
		Vec4f(2.0f * (qxy + qwz), 1.0f - 2.0f * (qxx + qzz), 2.0f * (qyz - qwx), 0.0f),
		Vec4f(2.0f * (qxz - qwy), 2.0f * (qyz + qwx), 1.0f - 2.0f * (qxx + qyy), 0.0f),
		Vec4f(0.0f, 0.0f, 0.0f, 1.0f)
	);

	return Result;

}

Mat4f MathUtils::lookAt(Vec4f eye, Vec4f center, Vec4f up)
{
	Vec4f const vf((center - eye).Normalize());
	Vec4f const vs((vf ^ up).Normalize());
	Vec4f const vu(vs ^ vf);

	float f[4]; vf.Store(f);
	float s[4]; vs.Store(s);
	float u[4]; vu.Store(u);

	Mat4f Result = Mat4f(
		Vec4f(s[0], s[1], -s[2], -(vs * eye).compSum()),
		Vec4f(u[0], u[1], -u[2], -(vu * eye).compSum()),
		Vec4f(f[0], f[1], -f[2],  (vf * eye).compSum()),
		Vec4f(   0,    0,     0,                     1)
	);
	return Result;
}

Quatf MathUtils::slerp(Quatf v0, Quatf v1, float ratio)
{
	//v0 = v0.Normalize();
	//v1 = v1.Normalize();
	//
	//double dot = (v0*v1).compSum();
	//if (dot < 0.0f) {
	//	v1 = -v1;
	//	dot = -dot;
	//}
	//if (dot > 0.9995) {
	//	Quatf result = v0 + ratio * (v1 - v0);
	//	return result.Normalize();
	//}
	//double theta_0 = acos(dot);
	//double theta = theta_0 * ratio;
	//double sin_theta_0 = sin(theta_0);
	//double sin_theta = sin(theta);
	//double s1 = sin_theta / sin_theta_0;
	//double s0 = cos(theta) -dot * s1;
	//return ((float)s0 * v0) + ((float)s1 * v1);

	Quatf z = v1;

	float cosTheta = v0.dot(v1);

	// If cosTheta < 0, the interpolation will take the long way around the sphere.
	// To fix this, one quat must be negated.
	if (cosTheta < 0.0f)
	{
		z = -v1;
		cosTheta = -cosTheta;
	}

	// Perform a linear interpolation when cosTheta is close to 1 to avoid side effect of sin(angle) becoming a zero denominator
	if (cosTheta > 1.0f - FLT_EPSILON)
	{
		return (v0 + (v1 - v0) * ratio).Normalize();
	}
	else
	{
		float angle = acos(cosTheta);
		return (sin((1.0f - ratio) * angle) * v0 + sin(ratio * angle) * z) / sin(angle);
	}
}
