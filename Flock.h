#pragma once

#include "bird.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <omp.h>

class Flock
{
public:
	int number_of_birds = 10000;
	Bird **birds;

	Bird ***grid;
	int *birds_per_grid;
	int cell_size;
	Dimension grid_size;
	Dimension window_size;

	Flock(int size);
	~Flock();

	void run();
	void generate(Dimension);
	void generateGrid(Dimension, int);
	void distributeBirds();
};

