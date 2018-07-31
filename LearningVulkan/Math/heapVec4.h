#pragma once
#include <malloc.h>
#include <iostream>

template<typename T, int align = alignof(T)>
struct heapAligned
{
	void* data = malloc(sizeof(T) + alignof(T) - 1);
	T* val = nullptr;

	heapAligned<T>()
	{
		if ((uintptr_t)data % align == 0)
			val = (T*)data;
		else
			val = data + (align + ((uintptr_t)data % align));

		printf("data at %p\nval aligned at %p\n", data, val);
	}
};