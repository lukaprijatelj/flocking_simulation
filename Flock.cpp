#include "Flock.h"
#include <time.h>


Flock::Flock(int size)
{
	number_of_birds = size;
}

Flock::~Flock()
{
}

void Flock::generate(Dimension windowDimension) {
	birds = new Bird*[number_of_birds];
	srand(time(NULL));

	// Layout birds on the canvas
	for (int i = 0; i < number_of_birds; i++) {
		birds[i] = new Bird(windowDimension);

		float x = float(rand()) / RAND_MAX * windowDimension.width;
		float y = float(rand()) / RAND_MAX * windowDimension.height;

		birds[i]->position.x = x - windowDimension.width / 2;
		birds[i]->position.y = y - windowDimension.height / 2;

		Vector velocityV = Vector();
		velocityV.x = rand();
		velocityV.y = rand();
		velocityV.normalize(birds[i]->MAX_SPEED);
		birds[i]->velocity = velocityV;
		birds[i]->rotate();

		//birds[i]->report();
	}
}

void Flock::generateFromNumberTable(Dimension windowDimension, float *table) {
	if (birds == NULL) {
		birds = new Bird*[number_of_birds];
	}

	// Layout birds on the canvas
	for (int i = 0, j=0; i < number_of_birds; i++) {
		birds[i] = new Bird(windowDimension);

		birds[i]->position.x = table[j++];
		birds[i]->position.y = table[j++];

		Vector velocityV = Vector();
		velocityV.x = table[j++];
		velocityV.y = table[j++];
		velocityV.normalize(birds[i]->MAX_SPEED);
		birds[i]->velocity = velocityV;
		birds[i]->rotate();

		//birds[i]->report();
	}
}

// Alocates and initializes the grid. Ment to only be called once
void Flock::generateGrid(Dimension windowDimension, int cell_size) {
	this->cell_size = cell_size;
	this->window_size = windowDimension;

	int num_width = ceil((float)(windowDimension.width) / cell_size);
	int num_height = ceil((float)(windowDimension.height) / cell_size);
	this->grid_size = Dimension(num_width, num_height);

	this->grid = new Bird**[num_width*num_height];
	this->birds_per_grid = new int[num_width*num_height];

	for (int i = 0; i < num_width*num_height; i++) {
		this->birds_per_grid[i] = 0;
		this->grid[i] = new Bird*[number_of_birds];
		for (int j = 0; j < number_of_birds; j++) {
			this->grid[i][j] = NULL;
		}
	}
}

void Flock::distributeBirds() {
	int i = 0;

	// Reset counts
	for (int i = 0; i < grid_size.width*grid_size.height; i++) {
		this->birds_per_grid[i] = 0;
	}

	// Distribute birds
	for (i = 0; i < number_of_birds; i++) {
		int cell_x = (birds[i]->position.x + (window_size.width / 2)) / cell_size;
		int cell_y = (birds[i]->position.y + (window_size.height / 2)) / cell_size;
		int grid_index = (cell_y * grid_size.width) + cell_x;

		// Place bird in grid and increment birds_per_grid
		this->grid[grid_index][birds_per_grid[grid_index]++] = birds[i];
	}
}

int Flock::run(int my_rank, int num_of_procesors) {
	// Calculate new birds positions
	int calculated = 0;
	for (int i = my_rank; i < number_of_birds; i+= num_of_procesors) {
		birds[i]->run(grid, birds_per_grid, grid_size, cell_size);
		calculated++;
	}
	return calculated;
}