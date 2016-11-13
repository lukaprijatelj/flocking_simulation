#include "Vector.h"



Vector::Vector()
{
}

Vector::Vector(float newX, float newY)
{
	x = newX;
	y = newY;
}

Vector::~Vector()
{
}

float Vector::length() {
	return sqrt(x * x + y * y);
}

void Vector::normalize(float number) {
	float length = this->length();

	if (length != 0) {
		x = x / length;
		y = y / length;

		x = x * number;
		y = y * number;
	}
}