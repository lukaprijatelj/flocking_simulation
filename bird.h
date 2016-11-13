#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include "Vector.h"
#include "Color.h"

#define LOCAL_DISTANCE 1
#define BIRD_SPEED 1

class Bird
{
private:
	Vector nextPosition;
	Vector nextVelocity;
	float nextRotation;

public:
	Vector position;
	Vector velocity;
	float rotation;
	Color color;
	Bird();
	~Bird();
	void report();

	void setX(float val);
	void setY(float val);
	void setVelX(float val);
	void setVelY(float val);
	void setRotation(float val);
	void setPosition(float x, float y);

	void swapNextValues();

	float getX();
	float getY();
	float getVelX();
	float getVelY();
	float getRotation();
	float getColorR();
	float getColorG();
	float getColorB();

	void compute_new_position(Bird **, int);

	float distanceFrom(Bird*);
};

