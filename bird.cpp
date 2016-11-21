#include "bird.h"
#include <stdio.h>


Bird::Bird(Dimension win) 
{
	window_dimensions = win;

	position = Vector();
	acceleration = Vector();
	velocity = Vector();
	rotation = 0.0f;

	color.red = 0.30f;
	color.green = 0.00f;
	color.blue = 1.00f;
}

Bird::~Bird() {}

void Bird::report() 
{
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

// Separation
// Method checks for nearby boids and steers away
Vector Bird::separate(Bird **birdArray, int n) 
{
	Vector steer = Vector(0, 0);
	int count = 0;

	// For every boid in the system, check if it's too close
	for (int i = 0; i < n; i++)
	{
		Bird *bird = birdArray[i];
		float d = Vector::distance(position, bird->position);

		// If the distance is 0, than this bird is same
		if (d == 0)
			continue;

		if (d < SEPARATION_DISTANCE) {
			// Calculate vector pointing away from neighbor
			Vector diff = Vector::subtract(position, bird->position);
			diff.normalize(1.0f);

			// Weight by distance
			diff.divide(d);        
			steer.add(diff);
			count++;           
		}
	}

	// Divide with how many birds are in local flock
	if (count > 0) {
		steer.divide((float)count);
	}

	if (steer.length() > 0) {
		// Implement Reynolds: 
		// Steering = Desired - Velocity
		steer.normalize(1.0f);
		steer.multiply(MAX_SPEED);
		steer.subtract(velocity);
		steer.limit(MAX_FORCE);
	}
	return steer;
}

// Alignment
// For every nearby boid in the system, calculate the average velocity
Vector Bird::align(Bird **birdArray, int n) 
{
	Vector sum = Vector(0, 0);
	int count = 0;

	for (int i = 0; i < n; i++)
	{
		Bird *bird = birdArray[i];
		float d = Vector::distance(position, bird->position);

		// If the distance is 0, than this bird is same
		if (d == 0)
			continue;

		if (d < ALLIGNMENT_DISTANCE) {
			sum.add(bird->velocity);
			count++;
		}
	}

	Vector steer = Vector();

	if (count > 0) {
		sum.divide((float)count);

		// Implement Reynolds: 
		// Steering = Desired - Velocity
		sum.normalize(1.0f);
		sum.multiply(MAX_SPEED);
		steer = Vector::subtract(sum, velocity);
		steer.limit(MAX_FORCE);
		
	}

	return steer;
}

// Cohesion
// For the average position (i.e. center) of all nearby boids, calculate steering vector towards that position
Vector Bird::cohesion(Bird **birdArray, int n) 
{
	Vector sum = Vector(0, 0);   
	int count = 0;

	for (int i = 0; i < n; i++)
	{
		Bird *bird = birdArray[i];
		float d = Vector::distance(position, bird->position);

		// If the distance is 0, than this bird is same
		if (d == 0)
			continue;

		if (d < COHESION_DISTANCE) {
			// Add position
			sum.add(bird->position); 
			count++;
		}
	}

	Vector steerVector = Vector();

	if (count > 0) {
		sum.divide(count);

		// Steer towards the position
		steerVector = seek(sum);
	}
	
	return steerVector;
}

Vector Bird::seek(Vector target)
{
	// A vector pointing from the position to the target
	Vector desired = Vector::subtract(target, position);  
						
	// Scale to maximum speed
	desired.normalize(1.0f);
	desired.multiply(MAX_SPEED);

	// Steering = Desired minus Velocity
	Vector steer = Vector::subtract(desired, velocity);

	// Limit to maximum steering force
	steer.limit(MAX_FORCE);  

	return steer;
}

// We accumulate a new acceleration each time based on three rules
void Bird::flock(Bird **birdArray, int n) 
{
	Vector sep = separate(birdArray, n);   
	Vector ali = align(birdArray, n);     
	Vector coh = cohesion(birdArray, n);  

	// Arbitrarily weight these forces
	sep.multiply(SEPARATION_WEIGHT);
	ali.multiply(ALLIGNMENT_WEIGHT);
	coh.multiply(COHESION_WEIGHT);

	// Add the force vectors to acceleration
	applyForce(sep);
	applyForce(ali);
	applyForce(coh);
}

void Bird::applyForce(Vector force) 
{
	acceleration.add(force);
}

void Bird::run(Bird **birdArray, int n) 
{
	// Calculate new position
	flock(birdArray, n);

	// Move bird
	update();

	// Rotate bird
	rotate();

	// Check for borders
	borders();
}

// Method to update position
void Bird::update() 
{
	// Update velocity
	velocity.add(acceleration);

	// Limit speed
	velocity.limit(MAX_SPEED);
	position.add(velocity);

	// Reset acceleration to 0 each cycle
	acceleration.multiply(0);
}

// Rotates bird to direction of velocity
void Bird::rotate() 
{
	Vector firstVector = Vector(1, 0);
	Vector secondVector = velocity;

	float dotProduct = 1 * secondVector.x + firstVector.y * secondVector.y;
	float determinant = 1 * secondVector.y - firstVector.y * secondVector.x;
	float GLFW_Exception = -90;

	rotation = (float)(atan2(determinant, dotProduct) * 180 / M_PI + GLFW_Exception);
}

void Bird::borders() 
{
	if (position.x < -(BIRD_RADIUS + window_dimensions.width / 2))
		position.x = window_dimensions.width / 2 + BIRD_RADIUS;
	if (position.y < -(BIRD_RADIUS + window_dimensions.height / 2))
		position.y = window_dimensions.height / 2 + BIRD_RADIUS;
	if (position.x > (window_dimensions.width / 2 + BIRD_RADIUS))
		position.x = -(BIRD_RADIUS + window_dimensions.width / 2);
	if (position.y > (window_dimensions.height / 2 + BIRD_RADIUS))
		position.y = -(BIRD_RADIUS + window_dimensions.height / 2);
}
