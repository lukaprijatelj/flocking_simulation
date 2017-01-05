//#include "glad.h"
//#include "GLFW/glfw3.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "graphics_manager.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "Flock.h"
#include <time.h>


using namespace std;

void print_stats(float avg_loop_time) {
	cout << '\r' << "Avg loop time: " << avg_loop_time << flush;
}

int main() {
	Graphics_manager *graphics_manager = new(Graphics_manager);
	Flock flock = Flock();
	Dimension window_dimension = graphics_manager->getDimensions();
	uint64_t loop_count = 0;

	flock.initGPU();

	// Generate all N number of birds and layout them on canvas.
	// Layout dimensions will be same as one of the window.
	flock.generate(window_dimension);

	flock.buildKernel();

	// Start iterations
	clock_t begin_time = clock();
	while (graphics_manager->loop()) 
	{
		loop_count++;
		graphics_manager->draw_birds(flock.birds, flock.number_of_birds);
		graphics_manager->swap_buffers();

		// Recalculates positions for all birds in the flock.
		flock.run();

		//print_stats(1.0f / (float(clock() - begin_time) / CLOCKS_PER_SEC / loop_count));
	}

	// Cleanup
	delete graphics_manager;

	system("pause");
	exit(EXIT_SUCCESS);
}