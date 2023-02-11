#pragma once

struct vect2d
{
	vect2d(int value1, int value2) : x{value1}, y{value2} {}
	vect2d() : vect2d(0, 0) {}
	vect2d(const vect2d& v) : vect2d(v.x, v.y) {}
	union
	{
		int x;
		int w;
	};
	union
	{
		int y;
		int h;
	};
};