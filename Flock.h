#pragma once

#include "bird.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <omp.h>

class Flock
{
public:
	int number_of_birds;
	Bird **birds;

	Bird ***grid;
	int *birds_per_grid;
	int cell_size;
	Dimension grid_size;
	Dimension window_size;

	Flock(int size);
	~Flock();
	void generate(Dimension);
	void generateFromNumberTable(Dimension windowDimension, float * table);
	void generateGrid(Dimension, int);
	void distributeBirds();
	int run(int my_rank, int num_of_procesors);
};

