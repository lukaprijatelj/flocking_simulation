#include "bird.h"
#include <stdio.h>


Bird::Bird() {
	for (int i = 0; i < 2; i++) {
		position[i] = 0.0f;
		velocity[i] = 0.0f;
	}
	color[0] = 0.30f; // R
	color[1] = 0.00f; // G
	color[2] = 1.00f; // B
	rotation = 0.0f;
}

Bird::~Bird(){}

void Bird::report(){
	printf("Bird vector: ");
	printf("[%.2f", position[0]);
	for (int i = 1; i < 2; i++) 
		printf(", %.2f", position[i]);
	printf("] velocity: [%.2f", velocity[0]);
	for (int i = 1; i < 2; i++)
		printf(", %.2f", velocity[i]);
	printf("] rotation: %.2f\n", rotation);
}

void Bird::setX(float val) {
	position[0] = val;
}

void Bird::setY(float val) {
	position[1] = val;
}

void Bird::setRotation(float val) {
	rotation = val;
}

void Bird::setPosition(float x, float y) {
	position[0] = x;
	position[1] = y;
}

float Bird::getX() {
	return position[0];
}

float Bird::getY() {
	return position[1];
}

float Bird::getVelX() {
	return velocity[0];
}

float Bird::getVelY() {
	return velocity[1];
}

float Bird::getRotation() {
	return rotation;
}

float Bird::getColorR() {
	return color[0];
}

float Bird::getColorG() {
	return color[1];
}

float Bird::getColorB() {
	return color[2];
}

float Bird::distance(Bird *other) {
	return sqrt(((position[0] - other->getX()) * (position[0] - other->getX())) + ((position[1] - other->getY()) * (position[1] - other->getY())));
}

void Bird::positionInFlock(Bird **birds, int n) {
	float alignment[2], cohesion[2], seperatation[2];
	int local_birds_count = 0;
	for (int  i = 0; i < 2; i++) {
		alignment[i] = 0.f;
		cohesion[i] = 0.f;
		seperatation[i] = 0.f;
	}

	for (int  i = 0; i < n; i++) {
		if (birds[i] != this) {
			if (distance(birds[i]) < LOCAL_DISTANCE) {
				local_birds_count++;

				alignment[0] += birds[i]->getVelX();
				alignment[1] += birds[i]->getVelY();

				cohesion[0] += birds[i]->getX();
				cohesion[1] += birds[i]->getY();

				seperatation[0] += birds[i]->getX() - getX();
				seperatation[1] += birds[i]->getY() - getY();
			}
		}
	}

	if (local_birds_count == 0)
		return;



	return;
}
