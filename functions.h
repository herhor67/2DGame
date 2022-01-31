#pragma once
#include "defines.h"

#include <algorithm>
#include <array>
//#include <bit>
#include <iostream>
#include <map>
#include <span>
#include <type_traits>
#include <unordered_set>
#include <vector>

#pragma warning(disable:4244)
#include "gcem.hpp"
#pragma warning(default:4244)


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

inline float  remap01(float ostart, float ostop, float value)
{
	return ostart + (ostop - ostart) * value;
}

template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
inline constexpr Integer remap01(Integer ostart, Integer ostop, float value)
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


inline constexpr float gauss_cdf(float x, float u, float o)
{
	return 0.5f * (1.0f + gcem::erf((x - u) / (o * (float)GCEM_SQRT_2)));
}

inline constexpr float gauss_pdf_dscrt(int x, int u, float o)
{
	return gauss_cdf(x + 0.5f, float(u), o) - gauss_cdf(x - 0.5f, float(u), o);
}


template <typename Floating, std::enable_if_t<std::is_floating_point_v<Floating>, bool> = true>
constexpr Floating nextafter(Floating x)
{
	return x / (Floating(1) - gcem::sqrt(std::numeric_limits<Floating>::epsilon()));
}


template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
constexpr auto integer_to_varint(Integer input)
{
	constexpr size_t byteCount = (sizeof(Integer) * CHAR_BIT + CHAR_BIT - 2) / (CHAR_BIT - 1);

	struct {
		std::array<uint8_t, byteCount> bytes;
		size_t index;
	} result{};

	typedef std::make_unsigned<Integer>::type UInteger;
	auto value = *(UInteger*)&input;

	for (size_t i = 1; i <= byteCount; ++i)
	{
		result.bytes[byteCount - i] |= value & (uint8_t(-1) >> 1);
		if (value >>= CHAR_BIT - 1)
			result.bytes[byteCount - i - 1] |= uint8_t(-1) & ~(uint8_t(-1) >> 1);
		else
		{
			result.index = byteCount - i;
			break;
		}
	}
	return result;
}