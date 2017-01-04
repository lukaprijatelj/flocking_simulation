#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include "Vector.h"
#include "Color.h"
#include "Dimension.h"






class Bird
{
public:
	


	

	Bird(Dimension);
	~Bird();

	// General
	void report();
	void rotate();
	void update();
	void applyForce(Vector);
	Vector seek(Vector);
	void borders();

	// Main calculation
	void calculate(Bird **, int);
};