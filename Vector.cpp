#include "Vector.h"



Vector::Vector()
{
	x = 0;
	y = 0;
}

Vector::Vector(float newX, float newY)
{
	x = newX;
	y = newY;
}

Vector::~Vector()
{
}

float Vector::length() 
{
	return sqrt(x * x + y * y);
}

void Vector::normalize(float number) 
{
	float length = this->length();

	if (length != 0) {
		x = x / length;
		y = y / length;

		x = x * number;
		y = y * number;
	}
}

void Vector::add(Vector add) 
{
	x = x + add.x;
	y = y + add.y;
}

void Vector::subtract(Vector subtract) 
{
	x = x - subtract.x;
	y = y - subtract.y;
}

Vector Vector::add(Vector a, Vector b) 
{
	Vector toReturn = Vector();
	toReturn.x = a.x + b.x;
	toReturn.y = a.y + b.y;
	return toReturn;
}

Vector Vector::subtract(Vector a, Vector b) 
{
	Vector toReturn = Vector();
	toReturn.x = a.x - b.x;
	toReturn.y = a.y - b.y;
	return toReturn;
}

float Vector::distance(Vector a, Vector b) 
{
	Vector subtracted = Vector();
	subtracted.x = a.x - b.x;
	subtracted.y = a.y - b.y;
	return sqrt(subtracted.x * subtracted.x + subtracted.y * subtracted.y);
}

void Vector::divide(float number) 
{
	x /= number;
	y /= number;
}

void Vector::multiply(float number) 
{
	x *= number;
	y *= number;
}

void Vector::limit(float number) 
{
	if (length() > number) 
	{
		normalize(number);
	}
}