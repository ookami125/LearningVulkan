#pragma once
#include "swizzle.h"
#include <intrin.h>
#include <string.h>
#include <stdint.h>
#include <iostream>
#include <assert.h>

struct alignas(16) Vec4f
{
	__m128 data;
	Vec4f()
	{
		assert(((const ptrdiff_t)this) % alignof(Vec4f) == 0);
		Load(0);
	}

	__forceinline Vec4f(float x)
	{
		assert(((const ptrdiff_t)this) % alignof(Vec4f) == 0);
		Load(x);
	}

	__forceinline Vec4f(float x, float y, float z, float w)
	{
		assert(((const ptrdiff_t)this) % alignof(Vec4f) == 0);
		Load(x, y, z, w);
	}

	__forceinline Vec4f(__m128 data) : data(data)
	{
		assert(((const ptrdiff_t)this) % alignof(Vec4f) == 0);
	}

	operator __m128() const { return data; }

	__forceinline void Load(float xyzw)
	{
		data = _mm_set_ps1(xyzw);
	}

	__forceinline void Load(float x, float y, float z, float w)
	{
		data = _mm_set_ps(w, x, y, z);
	}

	__forceinline void Store(float* x, float* y, float* z, float* w) const
	{
		alignas(16) float _data[4];
		_mm_store_ps(_data, data);
		*x = _data[0];
		*y = _data[1];
		*z = _data[2];
		*w = _data[3];
	}

	__forceinline void Store(float* xyzw) const
	{
		alignas(16) float _data[4];
		_mm_store_ps(_data, data);
		memcpy_s(xyzw, 4 * sizeof(float), _data, sizeof(data));
	}

	__forceinline float compSum()
	{
		float data[4]; Store(data);
		return data[0] + data[1] + data[2] + data[3];
	}

	template<int swizzle>
	__forceinline Vec4f Swizzle() const
	{
		return Shuffle<swizzle>(data);
	}

	template<int swizzle>
	__forceinline Vec4f Shuffle(const Vec4f & rhs) const
	{
		return _mm_shuffle_ps(data, rhs.data, swizzle);
	}

	__forceinline Vec4f operator+(const Vec4f & rhs) const
	{
		return _mm_add_ps(data, rhs.data);
	}

	__forceinline Vec4f operator-(const Vec4f & rhs) const
	{
		return _mm_sub_ps(data, rhs.data);
	}

	__forceinline Vec4f operator*(const Vec4f & rhs) const
	{
		return _mm_mul_ps(data, rhs.data);
	}

	__forceinline Vec4f operator/(const Vec4f & rhs) const
	{
		return _mm_div_ps(data, rhs.data);
	}

	__forceinline Vec4f operator^(const Vec4f & rhs) const
	{
		Vec4f c = (*this * rhs.Swizzle<YZXW>() - Swizzle<YZXW>() * rhs).Swizzle<YZXW>();
		return c;
	}

	__forceinline bool operator==(const Vec4f & rhs) const
	{
		__m128 vcmp = _mm_cmpeq_ps(data, rhs.data);
		unsigned char t[255];
		float* temp = (float*)(((uintptr_t)t >> 7) << 7);
		_mm_store_ps(temp, vcmp);
		return (temp[0] == 0.0f) && (temp[1] == 0.0f) && (temp[2] == 0.0f) && (temp[3] == 0.0f);
	}

	__forceinline Vec4f& operator+=(const Vec4f & rhs)
	{
		return *this = *this + rhs;
	}

	__forceinline Vec4f& operator-=(const Vec4f & rhs)
	{
		return *this = *this - rhs;
	}

	__forceinline Vec4f& operator*=(const Vec4f & rhs)
	{
		return *this = *this * rhs;
	}

	__forceinline Vec4f& operator/=(const Vec4f & rhs)
	{
		return *this = *this / rhs;
	}
};