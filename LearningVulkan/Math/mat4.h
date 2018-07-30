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
		Mat4f r;

		// transpose 3x3, we know m03 = m13 = m23 = 0
		Vec4f t0 = _mm_movelh_ps(row[0], row[1]); // 00, 01, 10, 11
		Vec4f t1 = _mm_movehl_ps(row[0], row[1]); // 02, 03, 12, 13
		r.row[0] = t0.Shuffle<_MM_SHUFFLE(0, 2, 0, 3)>(row[2]); //VecShuffle(t0, inM.mVec[2], 0, 2, 0, 3); // 00, 10, 20, 23(=0)
		r.row[1] = t0.Shuffle<_MM_SHUFFLE(1, 3, 1, 3)>(row[2]); //VecShuffle(t0, inM.mVec[2], 1, 3, 1, 3); // 01, 11, 21, 23(=0)
		r.row[2] = t1.Shuffle<_MM_SHUFFLE(1, 3, 1, 3)>(row[2]); //VecShuffle(t1, inM.mVec[2], 0, 2, 2, 3); // 02, 12, 22, 23(=0)

															 // (SizeSqr(mVec[0]), SizeSqr(mVec[1]), SizeSqr(mVec[2]), 0)
		Vec4f sizeSqr = ((((r.row[0] * r.row[0]) + r.row[1] * r.row[1])) + r.row[2] * r.row[2]);

		// optional test to avoid divide by 0
		Vec4f one = _mm_set1_ps(1.f);
		// for each component, if(sizeSqr < SMALL_NUMBER) sizeSqr = 1;
		Vec4f rSizeSqr = _mm_blendv_ps( one + sizeSqr, one, _mm_cmplt_ps(sizeSqr, Vec4f(1.e-8f)));

		r.row[0] *= rSizeSqr;
		r.row[1] *= rSizeSqr;
		r.row[2] *= rSizeSqr;

		// last line
		r.row[3] = r.row[0] + row[3].Swizzle<XXXX>();
		r.row[3] = r.row[3] + r.row[1] * row[3].Swizzle<YYYY>();
		r.row[3] = r.row[3] + r.row[2] * row[3].Swizzle<ZZZZ>();
		r.row[3] = Vec4f(0.f, 0.f, 0.f, 1.f) - r.row[3];

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