#

#include <iostream>
#include <iomanip>
#include <limits>

#include "defines.h"
#include "typedefs.h"

#include "Environment.h"


void Pos::cout() const
{
	std::cout << "XY(" << X << ";" << Y << ")" << std::endl;
}


EntCrd getDistSq(Pos p1, Pos p2)
{
	return (p1.X - p2.X) * (p1.X - p2.X) + (p1.Y - p2.Y) * (p1.Y - p2.Y);
}

EntCrd getDistSq(EntCrd d1, EntCrd d2)
{
	return d1 * d1 + d2 * d2;
}

/*
EntCrd getDist(Pos p1, Pos p2)
{
	return sqrt(pow(p1.X - p2.X, 2) + pow(p1.Y - p2.Y, 2));
}

EntCrd getDist(EntCrd d1, EntCrd d2)
{
	return sqrt(pow(d1, 2) + pow(d2, 2));
}
*/




std::pair<bool, Pos> findIntersect(Pos p1, Pos p2, Pos p3, Pos p4)
{
//	p1.cout();
//	p2.cout();
//	p3.cout();
//	p4.cout();
	EntCrd d = (p1.X - p2.X)*(p3.Y - p4.Y) - (p1.Y - p2.Y)*(p3.X - p4.X);

	if (d == 0) // collision impossible, division impossible, screw calculations
		return std::make_pair(false, Pos());

	EntCrd t = (p1.X - p3.X)*(p3.Y - p4.Y) - (p1.Y - p3.Y)*(p3.X - p4.X);
	EntCrd u = (p1.X - p3.X)*(p1.Y - p2.Y) - (p1.Y - p3.Y)*(p1.X - p2.X);

	EntCrd td = t / d;
	EntCrd ud = u / d;

	/*
	std::cout << "D: " << std::setprecision(17) << d << std::endl;
	std::cout << "T: " << std::setprecision(17) << t << std::endl;
	std::cout << "U: " << std::setprecision(17) << u << std::endl;
	std::cout << "TD: " << std::setprecision(17) << td << std::endl;
	std::cout << "UD: " << std::setprecision(17) << ud << std::endl;
	//*/

	if (td < 0 || td > 1 || ud < 0 || ud > 1) // absolute miss
		return std::make_pair(false, Pos());

//	if ((t == 0 || t == d) && (u == 0 || u == d)) // only endpoints on both lines
//		return std::make_pair(false, Pos());
	
	else // inside of at least one line, if not two
		return std::make_pair(true, p1 + (p2 - p1) * td);
}