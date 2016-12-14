#pragma once

#include "bird.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

class Flock
{
public:
	int number_of_birds = 1000;
	Bird **birds;

	Bird ***grid;
	int *birds_per_grid;
	int cell_size;
	Dimension grid_size;
	Dimension window_size;

	Flock();
	~Flock();

	void run();
	void generate(Dimension);
	void generateGrid(Dimension, int);
	void distributeBirds();
};

