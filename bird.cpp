#include "bird.h"
#include <stdio.h>


Bird::Bird() {
	position.x = 0.0f;
	position.y = 0.0f;
	nextPosition.x = 0.0f;
	nextPosition.y = 0.0f;

	velocity.x = 0.0f;
	velocity.y = 0.0f;
	nextVelocity.x = 0.0f;
	nextVelocity.y = 0.0f;

	color.red = 0.30f; 
	color.green = 0.00f;
	color.blue = 1.00f;

	rotation = 0.0f;
	nextRotation = 0.0f;
}

Bird::~Bird(){}

void Bird::report(){
	printf("Bird vector: ");

	// Position
	printf("[%.2f", position.x);
	printf(", %.2f", position.y);

	// Velocity
	printf("] velocity: [%.2f", velocity.x);
	printf(", %.2f", velocity.y);

	// Rotation
	printf("] rotation: %.2f\n", rotation);
}

void Bird::setX(float val) {
	position.x = val;
}

void Bird::setY(float val) {
	position.y = val;
}

void Bird::setVelX(float val) {
	velocity.x = val;
}

void Bird::setVelY(float val) {
	velocity.y = val;
}

void Bird::setRotation(float val) {
	rotation = val;
}

void Bird::setPosition(float x, float y) {
	position.x = x;
	position.y = y;
}

float Bird::getX() {
	return position.x;
}

float Bird::getY() {
	return position.y;
}

float Bird::getVelX() {
	return velocity.x;
}

float Bird::getVelY() {
	return velocity.y;
}

float Bird::getRotation() {
	return rotation;
}

float Bird::getColorR() {
	return color.red;
}

float Bird::getColorG() {
	return color.green;
}

float Bird::getColorB() {
	return color.blue;
}

float Bird::distanceFrom(Bird *other) {
	return sqrt(((position.x - other->getX()) * (position.x - other->getX())) + ((position.y - other->getY()) * (position.y - other->getY())));
}

void Bird::swapNextValues() {
	position = nextPosition;
	velocity = nextVelocity;
	rotation = nextRotation;
}

void Bird::compute_new_position(Bird **birdArray, int n) {
	Vector alignment = Vector();
	Vector cohesion = Vector();
	Vector separation = Vector();
	int local_birds_count = 0;

	for (int i = 0; i < n; i++)
	{
		Bird *bird = birdArray[i];

		if (bird != this)
		{
			if (distanceFrom(bird) < LOCAL_DISTANCE)
			{
				// allignment
				alignment.x += bird->velocity.x;
				alignment.y += bird->velocity.y;

				// cohesion
				cohesion.x += bird->position.x;
				cohesion.y += bird->position.y;

				// separation
				separation.x += bird->position.x - position.x;
				separation.y += bird->position.y - position.y;

				local_birds_count++;
			}
		}
	}

	// if no birds around than just move in current direction
	if (local_birds_count == 0) {
		nextPosition.x = position.x + velocity.x;
		nextPosition.y = position.y + velocity.y;
		return;
	}

	// allignment
	alignment.x /= local_birds_count;
	alignment.y /= local_birds_count;
	alignment.normalize(1);
	
	// cohesion
	cohesion.x /= local_birds_count;
	cohesion.y /= local_birds_count;
	cohesion = Vector(cohesion.x - position.x, cohesion.y - position.y);
	cohesion.normalize(1);

	// separation
	separation.x /= local_birds_count;
	separation.y /= local_birds_count;
	separation.x *= -1;
	separation.y *= -1;
	separation = Vector(separation.x - position.x, separation.y - position.y);
	separation.normalize(1.0f);

	// putting it all together
	nextVelocity.x = velocity.x + alignment.x * ALLIGNMENT_WEIGHT + cohesion.x *COHESION_WEIGHT + separation.x * SEPARATION_WEIGHT;
	nextVelocity.y = velocity.y + alignment.y *ALLIGNMENT_WEIGHT + cohesion.y *COHESION_WEIGHT + separation.y * SEPARATION_WEIGHT;
	nextVelocity.normalize(BIRD_SPEED);


	nextPosition.x = position.x + nextVelocity.x;
	nextPosition.y = position.y + nextVelocity.y;



	// calculate rotation for correct bird drawing
	if (nextVelocity.x == 0) {
		if (nextVelocity.y >= 0)
			nextRotation = 0;  // Ne sme biti
		else
			nextRotation = 270;
	}
	else
		nextRotation = atan(nextVelocity.y / nextVelocity.x) * 180 / M_PI;

	nextRotation -= 90;

	return;
}