#pragma once

#include "bird.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>


#define NUMBER_OF_THREADS 10

class Flock
{
public:
	int number_of_birds = 200;
	Bird **birds;
	pthread_t *threads;
	bool threadRunning[NUMBER_OF_THREADS];

	Flock();
	~Flock();

	void run();
	void generate(Dimension);
	void initThreads();
};

