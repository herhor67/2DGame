#pragma once

#include <array>
#include <iostream>
#include <map>
#include <span>
#include <unordered_set>
#include <vector>

#include "gcem.hpp"

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

//template <typename T, size_t N>
//std::unordered_set<T> get_unique(std::array<T, N> vec)
//{
//	std::unordered_set<T> s;
//	for (const T &i : vec)
//		s.insert(i);
//	return s;
//}
//
//template <typename T, size_t N>
//std::map<T, size_t> get_unique_counts(std::array<T, N> vec)
//{
//	std::map<T, size_t> m;
//	for (const T& i : vec)
//		++m[i];
//	return m;
//}

template<double u, float o>
constexpr float gauss_cdf(float x)
{
	return 0.5f * (1.0f + gcem::erf((x - u) / (o * GCEM_SQRT_2)));
}

template<float u, float o>
constexpr float gauss_pdf_dscrt(int x)
{
	return gauss_cdf<u,o>(x + 0.5f) - gauss_cdf<u, o>(x - 0.5f);
}