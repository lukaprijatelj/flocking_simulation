#pragma once

#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <CL\cl.h>
#include "Dimension.h"
#include <time.h>


#define WORKGROUP_SIZE	32  // number of threads in workgroup
#define MAX_SOURCE_SIZE	16384  // size of kernel.cl in Bytes (power of 2)
#define MAX_SPEED 0.133333;


struct Vector {
	cl_long x;
	cl_long y;
};
struct Bird {
	Vector position;
	Vector velocity;
	Vector acceleration;
	cl_long rotation;
};


class Flock
{
public:
	int number_of_birds = 200;
	Bird *birds;

	// OpenCL properties
	cl_int ret;
	cl_context context;
	cl_command_queue command_queue;
	cl_device_id	device_id[10];
	char *source_str;
	cl_program program;
	cl_kernel kernel;
	size_t local_item_size[1];
	size_t global_item_size[1];
	Dimension window_dimensions;
	cl_mem flock_obj;


	Flock();
	~Flock();

	void run();
	void generate(Dimension);
	void initGPU();
	void buildKernel();
	void reportBird(struct Bird);
};

