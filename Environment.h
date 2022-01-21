#pragma once

#include <utility>

#include "typedefs.h"
#include "defines.h"

struct Pos
{
	EntCrd X = 0;
	EntCrd Y = 0;

	Pos& operator+=(Pos const& obj)
	{
		X += obj.X;
		Y += obj.Y;
		return *this;
	}
	Pos& operator-=(Pos const& obj)
	{
		X -= obj.X;
		Y -= obj.Y;
		return *this;
	}
	template<typename N, typename = typename std::enable_if<std::is_arithmetic<N>::value, N>::type>
	Pos& operator*=(N const& k)
	{
		X *= k;
		Y *= k;
		return *this;
	}
	template<typename N, typename = typename std::enable_if<std::is_arithmetic<N>::value, N>::type>
	Pos& operator/=(N const& k)
	{
		X /= k;
		Y /= k;
		return *this;
	}

	Pos operator+(Pos const& obj) const
	{
		Pos temp(*this);
		temp += obj;
		return temp;
	}
	Pos operator-(Pos const& obj) const
	{
		Pos temp(*this);
		temp -= obj;
		return temp;
	}
	template<typename N, typename = typename std::enable_if<std::is_arithmetic<N>::value, N>::type>
	Pos operator*(N const& k) const
	{
		Pos temp(*this);
		temp *= k;
		return temp;
	}
	template<typename N, typename = typename std::enable_if<std::is_arithmetic<N>::value, N>::type>
	Pos operator/(N const& k) const
	{
		Pos temp(*this);
		temp /= k;
		return temp;
	}

	void cout() const;
};

typedef std::pair<Pos, Pos> Movement;

struct Vel
{
	float X = 0;
	float Y = 0;
};

struct Acc
{
	float X = 0;
	float Y = 0;
};

struct Res
{
	float X = 1;
	float Y = 1;
};

struct Frc
{
	float X = 0;
	float Y = 0;
};

struct Environment
{
	Vel vel;
	Acc acc;
	Res res;
};


EntCrd getDistSq(Pos, Pos);
EntCrd getDistSq(EntCrd, EntCrd);
//EntCrd getDist(Pos, Pos);
//EntCrd getDist(EntCrd, EntCrd);

std::pair<bool, Pos> findIntersect(Pos, Pos, Pos, Pos);