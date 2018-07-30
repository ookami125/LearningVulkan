#pragma once
#include "vec4.h"
#include "swizzle.h"
#include <intrin.h>
#include <string.h>
#include <assert.h>

union alignas(16) Quatf
{
	__m128 data;
	__forceinline Quatf()
	{
		assert(((const ptrdiff_t)this) % alignof(Quatf) == 0);
		Load(0);
	}

	__forceinline Quatf(float x)
	{
		Load(x);
	}

	__forceinline Quatf(float x, float y, float z, float w)
	{
		Load(w, x, y, z);
	}

	__forceinline Quatf(__m128 data) : data(data)
	{
	}

	operator __m128() const { return data; }

	__forceinline void Load(float wxyz)
	{
		data = _mm_set_ps1(wxyz);
	}

	__forceinline void Load(float w, float x, float y, float z)
	{
		data = _mm_set_ps(w, x, y, z);
	}

	__forceinline void Store(float* w, float* x, float* y, float* z) const
	{
		alignas(16) float _data[4];
		_mm_store_ps(_data, data);
		*w = _data[0];
		*x = _data[1];
		*y = _data[2];
		*z = _data[3];
	}

	__forceinline void Store(float* wxyz) const
	{
		alignas(16) float _data[4];
		_mm_store_ps(_data, data);
		memcpy_s(wxyz, 4 * sizeof(float), _data, sizeof(data));
	}

	__forceinline Quatf operator*(const Quatf & rhs) const
	{
		Vec4f a1123 = _mm_shuffle_ps(data, data, 0xE5);
		Vec4f a2231 = _mm_shuffle_ps(data, data, 0x7A);
		Vec4f b1000 = _mm_shuffle_ps(rhs, rhs, 0x01);
		Vec4f b2312 = _mm_shuffle_ps(rhs, rhs, 0x9E);
		Vec4f t1 = _mm_mul_ps(a1123, b1000);
		Vec4f t2 = _mm_mul_ps(a2231, b2312);
		Vec4f t12 = _mm_add_ps(t1, t2);
		const __m128i mask = _mm_set_epi32(0, 0, 0, 0x80000000);
		Vec4f t12m = _mm_xor_ps(t12, _mm_castsi128_ps(mask));
		Vec4f a3312 = _mm_shuffle_ps(data, data, 0x9F);
		Vec4f b3231 = _mm_shuffle_ps(rhs, rhs, 0x7B);
		Vec4f a0000 = _mm_shuffle_ps(data, data, 0x00);
		Vec4f t3 = _mm_mul_ps(a3312, b3231);
		Vec4f t0 = _mm_mul_ps(a0000, rhs);
		Vec4f t03 = _mm_sub_ps(t0, t3);
		return _mm_add_ps(t03, t12m);
	}

	__forceinline bool operator==(const Quatf & rhs) const
	{
		Vec4f vcmp = _mm_cmpeq_ps(data, rhs.data);
		alignas(16) float temp[4];
		vcmp.Store(temp);
		return (temp[0] == 0.0f) && (temp[1] == 0.0f) && (temp[2] == 0.0f) && (temp[3] == 0.0f);
	}
};