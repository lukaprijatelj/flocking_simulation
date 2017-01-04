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

void Bird::calculate(Bird ***grid, int *bird_counts, Dimension grid_size, int cell_size)
{
	Vector separation_steer = Vector(0, 0);
	Vector allignment_steer = Vector(0, 0);
	Vector cohesion_steer = Vector(0, 0);
	Vector allignment_sum = Vector(0, 0);
	Vector cohesion_sum = Vector(0, 0);
	int separation_count = 0;
	int allignment_count = 0;
	int cohesion_count = 0;

	int bird_x = (this->position.x + (this->window_dimensions.width / 2)) / cell_size;
	int bird_y = (this->position.y + (this->window_dimensions.height / 2)) / cell_size;
	

	// For every boid in the system, check if it's too close
	for (int y = -1; y <= 1; y++) {
		// Make sure cell_y is within bounds
		int cell_y = bird_y + y;
		if ((cell_y < 0) || (cell_y >= grid_size.height))
			continue;
		for (int x = -1; x <= 1; x++) {
			// Make sure cell_x is within bounds
			int cell_x = bird_x + x;
			if ((cell_x < 0) || (cell_x >= grid_size.width))
				continue;

			int grid_index = ((cell_y)* grid_size.width) + cell_x;
			int n = bird_counts[grid_index];

			for (int i = 0; i < n; i++) {
				Bird *bird = grid[grid_index][i];
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
					separation_steer.add(diff);
					separation_count++;
				}

				if (d < ALLIGNMENT_DISTANCE) {
					allignment_sum.add(bird->velocity);
					allignment_count++;
				}

				if (d < COHESION_DISTANCE) {
					// Add position
					cohesion_sum.add(bird->position);
					cohesion_count++;
				}
			}
		}
	}

	// Divide with how many birds are in local flock
	if (separation_count > 0) {
		separation_steer.divide((float)separation_count);
	}

	if (separation_steer.length() > 0) {
		// Implement Reynolds: 
		// Steering = Desired - Velocity
		separation_steer.normalize(1.0f);
		separation_steer.multiply(MAX_SPEED);
		separation_steer.subtract(velocity);
		separation_steer.limit(MAX_FORCE);
	}
	
	if (allignment_count > 0) {
		allignment_sum.divide((float)allignment_count);

		// Implement Reynolds: 
		// Steering = Desired - Velocity
		allignment_sum.normalize(1.0f);
		allignment_sum.multiply(MAX_SPEED);
		allignment_steer = Vector::subtract(allignment_sum, velocity);
		allignment_steer.limit(MAX_FORCE);
	}

	if (cohesion_count > 0) {
		cohesion_sum.divide(cohesion_count);

		// Steer towards the position
		cohesion_steer = seek(cohesion_sum);
	}

	// Arbitrarily weight these forces
	separation_steer.multiply(SEPARATION_WEIGHT);
	allignment_steer.multiply(ALLIGNMENT_WEIGHT);
	cohesion_steer.multiply(COHESION_WEIGHT);

	// Add the force vectors to acceleration
	applyForce(separation_steer);
	applyForce(allignment_steer);
	applyForce(cohesion_steer);
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

void Bird::applyForce(Vector force) 
{
	acceleration.add(force);
}

void Bird::run(Bird ***grid, int *bird_counts, Dimension grid_size, int cell_size)
{
	// Calculate new position
	calculate(grid, bird_counts, grid_size, cell_size);

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
