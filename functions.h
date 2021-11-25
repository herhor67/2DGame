#pragma once

#include <iostream>
#include <vector>

#include "defines.h"
#include "typedefs.h"




void drawStringOnWindow(float, float, float, void*, const char*, int = 2, int = 10);

void drawStringOnWindow(float, float, float, void*, const std::string&, int = 2, int = 10);

void drawStringOnWindow(float, float, float, void*, const std::vector<std::string>&, int = 2, int = 10);


template <typename T> int sgn(T val)
{
	return (T(0) < val) - (val < T(0));
}

template <typename T> std::vector<T> interpolate(T v1, T v2, size_t count)
{
	std::vector<T> temp(count);
	
	T diff = v2 - v1;
	--count;

	for (size_t i = 0; i <= count; ++i)
	{
		temp[i] = v1 + diff * i / count;
	}
	temp[count] = v2;
	
	return temp;
}
