#pragma once
#include "defines.h"

#include <algorithm>
#include <array>
#include <iostream>
#include <map>
#include <span>
#include <unordered_set>
#include <vector>

#include "gcem.hpp"





void drawStringOnWindow(float, float, float, void*, const char*, int = 2, int = 10);

void drawStringOnWindow(float, float, float, void*, const std::string&, int = 2, int = 10);

void drawStringOnWindow(float, float, float, void*, const std::vector<std::string>&, int = 2, int = 10);


template <typename T>
int sgn(T val)
{
	return (T(0) < val) - (val < T(0));
}

template <typename T>
std::vector<T> interpolate(T v1, T v2, size_t count)
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


inline float  remap(float istart, float istop, float ostart, float ostop, float value)
{
	return ostart + (ostop - ostart) * ((value - istart) / (istop - istart));
}

inline BlkCrd remap(BlkCrd istart, BlkCrd istop, BlkCrd ostart, BlkCrd ostop, BlkCrd value)
{
	return ostart + (ostop - ostart) * (value - istart) / (istop - istart);
}

inline float  remap01_cnt(float ostart, float ostop, float value)
{
	return ostart + (ostop - ostart) * value;
}

template <typename Integer, std::enable_if_t<std::is_integral<Integer>::value, bool> = true>
inline constexpr Integer remap01_dsc(Integer ostart, Integer ostop, float value)
{
	return std::clamp(ostart + Integer((ostop - ostart + 1) * value), ostart, ostop);
}

template <typename N, size_t L>
constexpr N median(std::array<N, L> values)
{
	const auto middleItr = values.begin() + values.size() / 2;
	std::nth_element(values.begin(), middleItr, values.end());
	if (values.size() % 2 == 0)
	{
		const auto leftMiddleItr = std::max_element(values.begin(), middleItr);
		return (*leftMiddleItr + *middleItr) / 2;
	}
	else
		return *middleItr;
}


template<float u, float o>
constexpr float gauss_cdf(float x)
{
	return 0.5f * (1.0f + gcem::erf((x - u) / (o * GCEM_SQRT_2)));
}

template<int u, float o>
constexpr float gauss_pdf_dscrt(int x)
{
	return gauss_cdf<float(u), o>(x + 0.5f) - gauss_cdf<float(u), o>(x - 0.5f);
}


template <typename Floating, std::enable_if_t<std::is_floating_point<Floating>::value, bool> = true>
constexpr Floating nextafter(Floating x)
{
	return x / (Floating(1) - std::numeric_limits<Floating>::epsilon());
}

