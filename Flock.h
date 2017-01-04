#pragma once

#include "bird.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>
#include <CL/cl.h>


#define WORKGROUP_SIZE	32  // number of threads in workgroup
#define MAX_SOURCE_SIZE	16384  // size of kernel.cl in Bytes (power of 2)


struct Bird {
	float position[2];
	float velocity[2];
	float acceleration[2];
	float rotation;
};


class Flock
{
public:
	int number_of_birds = 200;
	Bird **birds;
	pthread_t *threads;

	// OpenCL properties
	cl_int ret;
	cl_context context;
	cl_command_queue command_queue;
	cl_device_id	device_id[10];
	char *source_str;
	cl_program program;
	cl_kernel kernel;
	size_t local_item_size[2];
	size_t global_item_size[2];
	Dimension window_dimensions;


	Flock();
	~Flock();

	void run();
	void generate(Dimension);
	void initGPU();
	void buildKernel();
	void reportBird(struct Bird);
};

