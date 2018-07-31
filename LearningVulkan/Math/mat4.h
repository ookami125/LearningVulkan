#pragma once
#include "vec4.h"
#include <assert.h>

union alignas(32) Mat4f
{
	Vec4f row[4];
	__m256 drow[2];
	__forceinline Mat4f()
	{
		assert(((const ptrdiff_t)this) % alignof(Mat4f) == 0);
		Load(1, 1, 1, 1);
	}

	__forceinline Mat4f(float x)
	{
		assert(((const ptrdiff_t)this) % alignof(Mat4f) == 0);
		Load(x, x, x, x);
	}

	__forceinline Mat4f(float x, float y, float z, float w)
	{
		assert(((const ptrdiff_t)this) % alignof(Mat4f) == 0);
		Load(x, y, z, w);
	}

	__forceinline Mat4f(Vec4f data[4])
	{
		assert(((const ptrdiff_t)this) % alignof(Mat4f) == 0);
		row[0] = data[0];
		row[1] = data[1];
		row[2] = data[2];
		row[3] = data[3];
	}

	__forceinline Mat4f(Vec4f r1, Vec4f r2, Vec4f r3, Vec4f r4)
	{
		assert(((const ptrdiff_t)this) % alignof(Mat4f) == 0);
		row[0] = r1;
		row[1] = r2;
		row[2] = r3;
		row[3] = r4;
	}

	__forceinline void Load(float d1, float d2, float d3, float d4)
	{
		row[0] = _mm_set_ps(0, 0, 0, d1);
		row[1] = _mm_set_ps(0, 0, d2, 0);
		row[2] = _mm_set_ps(0, d3, 0, 0);
		row[3] = _mm_set_ps(d4, 0, 0, 0);
	}

	__forceinline void Store(Vec4f* r1, Vec4f* r2, Vec4f* r3, Vec4f* r4) const
	{
		*r1 = row[0];
		*r2 = row[1];
		*r3 = row[2];
		*r4 = row[3];
	}

	Mat4f Inverse()
	{
		Vec4f A = row[0].Shuffle<XYXY>(row[1]);
		Vec4f B = row[0].Shuffle<ZWZW>(row[1]);
		Vec4f C = row[2].Shuffle<XYXY>(row[3]);
		Vec4f D = row[2].Shuffle<ZWZW>(row[3]);
		Vec4f detSub = row[0].Shuffle<XZXZ>(row[2]) * row[1].Shuffle<YWYW>(row[3]) - row[0].Shuffle<YWYW>(row[2]) * row[1].Shuffle<XZXZ>(row[3]);
		Vec4f detA = detSub.Swizzle<XXXX>();
		Vec4f detB = detSub.Swizzle<YYYY>();
		Vec4f detC = detSub.Swizzle<ZZZZ>();
		Vec4f detD = detSub.Swizzle<WWWW>();
		Vec4f D_C = D.Swizzle<WWXX>() * C - D.Swizzle<YYZZ>() * C.Swizzle<ZWXY>();
		Vec4f A_B = A.Swizzle<WWXX>() * B - A.Swizzle<YYZZ>() * B.Swizzle<ZWXY>();
		Vec4f X_ = detD * A - (B * D_C.Swizzle<XWXW>() + B.Swizzle<YXWZ>() * D_C.Swizzle<ZYZY>());
		Vec4f W_ = detA * D - (C * A_B.Swizzle<XWXW>() + C.Swizzle<YXWZ>() * A_B.Swizzle<ZYZY>());
		Vec4f Y_ = detB * C - (D * A_B.Swizzle<WXWX>() - D.Swizzle<YXWZ>() * A_B.Swizzle<ZYZY>());
		Vec4f Z_ = detC * B - (A * D_C.Swizzle<WXWX>() - A.Swizzle<YXWZ>() * D_C.Swizzle<ZYZY>());
		Vec4f tr = A_B * D_C.Swizzle<XZYW>();
		tr = _mm_hadd_ps(tr, tr);
		tr = _mm_hadd_ps(tr, tr);
		Vec4f rDetM = Vec4f(1.f, -1.f, -1.f, 1.f) / (detA * detD + detB * detC - tr);
		X_ *= rDetM;
		Y_ *= rDetM;
		Z_ *= rDetM;
		W_ *= rDetM;
		Mat4f r;
		r.row[0] = X_.Shuffle<WYWY>(Y_);
		r.row[1] = X_.Shuffle<ZXZX>(Y_);
		r.row[2] = Z_.Shuffle<WYWY>(W_);
		r.row[3] = Z_.Shuffle<ZXZX>(W_);
		return r;
	}

	Mat4f Transpose()
	{
		Mat4f temp = Mat4f(row);
		_MM_TRANSPOSE4_PS(temp.row[0], temp.row[1], temp.row[2], temp.row[3]);
		return temp;
	}

	__forceinline Mat4f operator*(const Mat4f rhs) const
	{
		Mat4f temp;
		_mm256_zeroupper();
		temp.drow[0] = _mm256_mul_ps(_mm256_shuffle_ps(drow[0], drow[0], 0x00), _mm256_broadcast_ps(&rhs.row[0].data));
		temp.drow[0] = _mm256_add_ps(temp.drow[0], _mm256_mul_ps(_mm256_shuffle_ps(drow[0], drow[0], 0x55), _mm256_broadcast_ps(&rhs.row[1].data)));
		temp.drow[0] = _mm256_add_ps(temp.drow[0], _mm256_mul_ps(_mm256_shuffle_ps(drow[0], drow[0], 0xaa), _mm256_broadcast_ps(&rhs.row[2].data)));
		temp.drow[0] = _mm256_add_ps(temp.drow[0], _mm256_mul_ps(_mm256_shuffle_ps(drow[0], drow[0], 0xff), _mm256_broadcast_ps(&rhs.row[3].data)));
		temp.drow[1] = _mm256_mul_ps(_mm256_shuffle_ps(drow[1], drow[1], 0x00), _mm256_broadcast_ps(&rhs.row[0].data));
		temp.drow[1] = _mm256_add_ps(temp.drow[1], _mm256_mul_ps(_mm256_shuffle_ps(drow[1], drow[1], 0x55), _mm256_broadcast_ps(&rhs.row[1].data)));
		temp.drow[1] = _mm256_add_ps(temp.drow[1], _mm256_mul_ps(_mm256_shuffle_ps(drow[1], drow[1], 0xaa), _mm256_broadcast_ps(&rhs.row[2].data)));
		temp.drow[1] = _mm256_add_ps(temp.drow[1], _mm256_mul_ps(_mm256_shuffle_ps(drow[1], drow[1], 0xff), _mm256_broadcast_ps(&rhs.row[3].data)));
		return temp;
	}

	__forceinline Vec4f operator*(const Vec4f rhs) const
	{
		__m128 m0 = _mm_mul_ps(row[0].data, rhs.data);
		__m128 m1 = _mm_mul_ps(row[1].data, rhs.data);
		__m128 m2 = _mm_mul_ps(row[2].data, rhs.data);
		__m128 m3 = _mm_mul_ps(row[3].data, rhs.data);
		__m128 sum_01 = _mm_hadd_ps(m0, m1);
		__m128 sum_23 = _mm_hadd_ps(m2, m3);
		return _mm_hadd_ps(sum_01, sum_23);
	}
};