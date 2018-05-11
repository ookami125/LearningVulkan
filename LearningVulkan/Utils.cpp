#include "Utils.h"
#include <Windows.h>
#include <stdint.h>

float Q_rsqrt(float number)
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y = number;
	i = *(long *)&y;                       // evil floating point bit level hacking
	i = 0x5f3759df - (i >> 1);               // what the fuck? 
	y = *(float *)&i;
	y = y * (threehalfs - (x2 * y * y));   // 1st iteration
										   //	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

	return y;
}

uint32_t nextPowerOfTwo(uint32_t v)
{
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;
	return v;
}

void* alignedAlloc(size_t size, size_t alignment)
{
	if ((alignment & (alignment - 1)) != 0)
		alignment = nextPowerOfTwo(alignment);
	void *data = nullptr;
	data = _aligned_malloc(size, alignment);
	return data;
}

void alignedFree(void* data)
{
	_aligned_free(data);
}