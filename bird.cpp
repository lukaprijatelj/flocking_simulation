#include "bird.h"
#include <stdio.h>



Bird::Bird(Dimension win) 
{
	window_dimensions = win;

	rotation = 0.0f;

	
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

