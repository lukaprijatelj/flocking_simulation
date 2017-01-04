#pragma once

#include "bird.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>

#define NUMBER_OF_THREADS 2

struct calculateParameters {
	// Grid
	Bird ***grid;
	int *bird_counts;
	Dimension grid_size;
	int cell_size;
	// Old
	int n;
	int threadIndex;
	Bird **birdArray;
};

class Flock
{
public:
	int number_of_birds = 1500;
	Bird **birds;

	Bird ***grid;
	int *birds_per_grid;
	int cell_size;
	Dimension grid_size;
	Dimension window_size;

	pthread_t threads[NUMBER_OF_THREADS];
	struct calculateParameters threadParams[NUMBER_OF_THREADS];

	Flock();
	Flock(int size);
	~Flock();

	void run();
	void generate(Dimension);
	void generateGrid(Dimension, int);
	void distributeBirds();
};

