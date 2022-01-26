#pragma once

#include <chrono>


inline std::string duration2readable(std::chrono::steady_clock::time_point beg, std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now())
{
	std::chrono::nanoseconds time_left = end - beg;

	std::chrono::days    d = std::chrono::duration_cast<std::chrono::days   >(time_left);
	time_left -= d;
	std::chrono::hours   h = std::chrono::duration_cast<std::chrono::hours  >(time_left);
	time_left -= h;
	std::chrono::minutes m = std::chrono::duration_cast<std::chrono::minutes>(time_left);
	time_left -= m;
	std::chrono::seconds s = std::chrono::duration_cast<std::chrono::seconds>(time_left);
	time_left -= s;
	std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_left);
	time_left -= ms;
	std::chrono::microseconds us = std::chrono::duration_cast<std::chrono::microseconds>(time_left);
	time_left -= us;
	std::chrono::nanoseconds  ns = std::chrono::duration_cast<std::chrono::nanoseconds >(time_left);

	int  dc =  d.count();
	int  hc =  h.count();
	int  mc =  m.count();
	int  sc =  s.count();
	int msc = ms.count();
	int usc = us.count();
	int nsc = ns.count();

	std::stringstream ss;
	ss.fill('0');

	if (dc)
		ss << std::setw(2) << dc << "d " << std::setw(2) << hc << "h ";
	else if (hc)
		ss << std::setw(2) << hc << "h " << std::setw(2) << mc << "m ";
	else if (mc)
		ss << std::setw(2) << mc << "m " << std::setw(2) << sc << "s ";
	else if (sc)
		ss << std::setw(2) << sc << "s " << std::setw(3) << msc << "ms ";
	else if (msc)
		ss << std::setw(3) << msc << "ms " << std::setw(3) << usc << "us ";
	else if (usc)
		ss << std::setw(3) << usc << "us " << std::setw(3) << nsc << "ns ";
	else if (nsc)
		ss << std::setw(3) << nsc << "ns ";
	else
		ss << std::setw(3) << 0 << "fs ";

	return ss.str();
}