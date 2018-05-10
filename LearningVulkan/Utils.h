#pragma once

float Q_rsqrt(float number);

void* alignedAlloc(size_t size, size_t alignment);

void alignedFree(void* data);