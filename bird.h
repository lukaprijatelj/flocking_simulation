#pragma once
#include <math.h>

#define LOCAL_DISTANCE 0.5

class Bird
{
	float position[2];
	float velocity[2];
	float rotation;
	float color[3];
public:
	Bird();
	~Bird();
	void report();

	void setX(float val);
	void setY(float val);
	void setRotation(float val);
	void setPosition(float x, float y);

	float getX();
	float getY();
	float getVelX();
	float getVelY();
	float getRotation();
	float getColorR();
	float getColorG();
	float getColorB();

	float distance(Bird*);

	void positionInFlock(Bird**, int);
};

