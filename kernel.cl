// Weights for birds to fly properly
#define ALLIGNMENT_WEIGHT 1.0 
#define COHESION_WEIGHT 1.0 
#define SEPARATION_WEIGHT 1.5 

// Other constants
#define SEPARATION_DISTANCE 25.0 
#define ALLIGNMENT_DISTANCE 50.0 
#define COHESION_DISTANCE 50.0 

#define MAX_SPEED = 2.0 / 15;
#define MAX_FORCE = 0.03 / 10;
#define BIRD_RADIUS = 5.0f;  // Radius of the bird (I have choose MAX point of bird triangle points)





// Structure definition
typedef struct {
	float x;
	float y;
} Vector;
typedef struct {
	Vector position;
	Vector velocity;
	Vector acceleration;
	float rotation;
} Bird;







// Calulates the new position
void Bird_calculate(Bird *currentBird, Bird **birdArray, int n)
{
	Vector separation_steer;
	separation_steer.x = 0;
	separation_steer.y = 0;

	Vector allignment_steer;
	allignment_steer.x = 0;
	allignment_steer.y = 0;

	Vector cohesion_steer;
	cohesion_steer.x = 0;
	cohesion_steer.y = 0;

	Vector allignment_sum;
	allignment_sum.x = 0;
	allignment_sum.y = 0;

	Vector cohesion_sum;
	cohesion_sum.x = 0;
	cohesion_sum.y = 0;

	int separation_count = 0;
	int allignment_count = 0;
	int cohesion_count = 0;

	// For every boid in the system, check if it's too close
	for (int i = 0; i < n; i++)
	{
		Bird *bird = birdArray[i];
		float d = Vector_distance(currentBird->position, bird->position);

		// If the distance is 0, than this bird is same
		if (d == 0)
			continue;

		if (d < SEPARATION_DISTANCE) {
			// Calculate vector pointing away from neighbor
			Vector diff = Vector_subtract(currentBird->position, bird->position);
			diff = Vector_normalize(diff, 1.0f);

			// Weight by distance
			diff = Vector_divide(diff, d);
			separation_steer = Vector_add(separation_steer, diff);
			separation_count++;
		}

		if (d < ALLIGNMENT_DISTANCE) {
			allignment_sum = Vector_add(allignment_sum, bird->velocity);
			allignment_count++;
		}

		if (d < COHESION_DISTANCE) {
			// Add position
			cohesion_sum = Vector_add(cohesion_sum, bird->position);
			cohesion_count++;
		}
	}

	// Divide with how many birds are in local flock
	if (separation_count > 0) {
		separation_steer = Vector_divide(separation_steer, (float)separation_count);
	}

	if (Vector_length(separation_steer) > 0) {
		// Implement Reynolds: 
		// Steering = Desired - Velocity
		separation_steer = Vector_normalize(separation_steer, 1.0f);
		separation_steer = Vector_multiply(separation_steer, MAX_SPEED);
		separation_steer = Vector_subtract(separation_steer, currentBird->velocity);
		separation_steer = Vector_limit(separation_steer, MAX_FORCE);
	}

	if (allignment_count > 0) {
		allignment_sum = Vector_divide(allignment_sum, (float)allignment_count);

		// Implement Reynolds: 
		// Steering = Desired - Velocity
		allignment_sum = Vector_normalize(allignment_sum, 1.0f);
		allignment_sum = Vector_multiply(allignment_sum, MAX_SPEED);
		allignment_steer = Vector_subtract(allignment_sum, currentBird->velocity);
		allignment_steer = Vector_limit(allignment_steer, MAX_FORCE);
	}

	if (cohesion_count > 0) {
		cohesion_sum = Vector_divide(cohesion_sum, cohesion_count);

		// Steer towards the position
		cohesion_steer = Bird_seek(currentBird, cohesion_sum);
	}

	// Arbitrarily weight these forces
	separation_steer = Vector_multiply(separation_steer, SEPARATION_WEIGHT);
	allignment_steer = Vector_multiply(allignment_steer, ALLIGNMENT_WEIGHT);
	cohesion_steer = Vector_multiply(cohesion_steer, COHESION_WEIGHT);

	// Add the force vectors to acceleration
	applyForce(currentBird, separation_steer);
	applyForce(currentBird, allignment_steer);
	applyForce(currentBird, cohesion_steer);
}
Vector Bird_seek(Bird *currentBird, Vector target)
{
	// A vector pointing from the position to the target
	Vector desired = Vector_subtract(target, currentBird->position);

	// Scale to maximum speed
	desired = Vector_normalize(desired, 1.0f);
	desired = Vector_multiply(desired, MAX_SPEED);

	// Steering = Desired minus Velocity
	Vector steer = Vector_subtract(desired, currentBird->velocity);

	// Limit to maximum steering force
	steer = Vector_limit(steer, MAX_FORCE);

	return steer;
}
void Bird_applyForce(Bird *currentBird, Vector force)
{
	currentBird->acceleration = Vector_add(currentBird->acceleration, force);
}
// Updates or moves the bird with precalculated velocity.
void Bird_update(Bird *currentBird)
{
	// Update velocity
	currentBird->velocity = Vector_add(currentBird->velocity, acceleration);

	// Limit speed
	currentBird->velocity = Vector_limit(currentBird->velocity, MAX_SPEED);
	currentBird->position = Vector_add(currentBird->position, velocity);

	// Reset acceleration to 0 each cycle
	currentBird->acceleration = Vector_multiply(currentBird->acceleration, 0);
}
// Rotates bird to direction of velocity
void Bird_rotate(Bird *currentBird)
{
	Vector firstVector;
	firstVector.x = 1;
	firstVector.y = 0;
	Vector secondVector = currentBird->velocity;

	float dotProduct = 1 * secondVector.x + firstVector.y * secondVector.y;
	float determinant = 1 * secondVector.y - firstVector.y * secondVector.x;
	float GLFW_Exception = -90;

	currentBird->rotation = (float)(atan2(determinant, dotProduct) * 180 / M_PI + GLFW_Exception);
}
void Bird_borders(Bird *currentBird)
{
	if (currentBird->position.x < -(BIRD_RADIUS + window_dimensions.width / 2))
		currentBird->position.x = window_dimensions.width / 2 + BIRD_RADIUS;
	if (currentBird->position.y < -(BIRD_RADIUS + window_dimensions.height / 2))
		currentBird->position.y = window_dimensions.height / 2 + BIRD_RADIUS;
	if (currentBird->position.x >(window_dimensions.width / 2 + BIRD_RADIUS))
		currentBird->position.x = -(BIRD_RADIUS + window_dimensions.width / 2);
	if (currentBird->position.y >(window_dimensions.height / 2 + BIRD_RADIUS))
		currentBird->position.y = -(BIRD_RADIUS + window_dimensions.height / 2);
}




float Vector_length(Vector vec)
{
	return sqrt(vec.x * vec.x + vec.y * vec.y);
}
Vector Vector_normalize(Vector vec, float number)
{
	float length = Vector_length(vec);

	Vector toReturn;

	if (length != 0) {
		toReturn.x = vec.x / length;
		toReturn.y = vec.y / length;

		toReturn.x = vec.x * number;
		toReturn.y = vec.y * number;
	}

	return toReturn;
}
Vector Vector_add(Vector a, Vector b)
{
	Vector toReturn;
	toReturn.x = a.x + b.x;
	toReturn.y = a.y + b.y;
	return toReturn;
}
Vector Vector::subtract(Vector a, Vector b)
{
	Vector toReturn;
	toReturn.x = a.x - b.x;
	toReturn.y = a.y - b.y;
	return toReturn;
}
float Vector_distance(Vector a, Vector b)
{
	Vector subtracted;
	subtracted.x = a.x - b.x;
	subtracted.y = a.y - b.y;
	return sqrt(subtracted.x * subtracted.x + subtracted.y * subtracted.y);
}
Vector Vector_divide(Vector vec, float number)
{
	Vector toReturn;
	toReturn.x = vec.x / number;
	toReturn.y = vec.y / number;
	return toReturn;
}
Vector Vector_multiply(Vector vec, float number)
{
	Vector toReturn;
	toReturn.x = vec.x * number;
	toReturn.y = vec.y * number;
	return toReturn;
}
Vector Vector_limit(Vector vec, float number)
{
	if (Vector_length(vec) > number)
	{
		return Vector_normalize(vec, number);
	}
	return vec;
}





// Main kernel program
__kernel void flockingSimulation(__global int *image,
	__global int *surface,
	int maxGray)
{
	// Global ids
	int gid_0 = get_global_id(0);
	int gid_1 = get_global_id(1);

	int global_size_0 = get_global_size(0);

	// Local ids
	//int lid_0 = get_local_id(0);
	//int lid_1 = get_local_id(1);



	// Iterate over threads birds
	if(gid_0 < global_size_0) {
		Bird *currentBird = input->birdArray[gid_0];

		// Calculate new position
		currentBird->calculate(&currentBird, input->birdArray, input->n);

		// Move bird
		currentBird->update(&currentBird);

		// Rotate bird
		currentBird->rotate(&currentBird);

		// Check for borders
		currentBird->borders(&currentBird);
	}


}