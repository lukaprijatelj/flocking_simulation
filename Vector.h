#pragma once

#include <math.h>

class Vector
{
public:
	Vector();
	Vector(float, float);
	~Vector();

	void normalize(float);

	float length();
	float x = 0;
	float y = 0;
};

