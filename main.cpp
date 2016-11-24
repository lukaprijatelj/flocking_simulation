//#include "glad.h"
//#include "GLFW/glfw3.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include "bird.h"
#include "graphics_manager.h"
#include "Vector.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "Flock.h"


using namespace std;

void print_stats(double calculation_time, double draw_time, float fps) {
	cout << '\r' << "Avg loop time: " << fps << " [calculation: " << calculation_time << "s] [draw: " << draw_time << "s]           " << flush;
}

int main() {
	Graphics_manager *graphics_manager = new(Graphics_manager);
	Flock flock = Flock();
	Dimension window_dimension = graphics_manager->getDimensions();
	uint64_t loop_count = 0;


	// Generate all N number of birds and layout them on canvas.
	// Layout dimensions will be same as one of the window.
	flock.generate(window_dimension);


	// Start iterations
	clock_t begin_time = clock();
	while (graphics_manager->loop()) 
	{
		loop_count++;

		clock_t begin_draw_time = clock();
		graphics_manager->draw_birds(flock.birds, flock.number_of_birds);
		graphics_manager->swap_buffers();
		clock_t end_draw_time = clock();
		
		// Recalculates positions for all birds in the flock.
		clock_t begin_calculation_time = clock();
		flock.run();
		clock_t end_calculation_time = clock();

		double calculation_time = double(end_calculation_time - begin_calculation_time) / CLOCKS_PER_SEC;
		double draw_time = double(end_draw_time - begin_draw_time) / CLOCKS_PER_SEC;
		float fps = 1.0f / (float(clock() - begin_time) / CLOCKS_PER_SEC / loop_count);

		print_stats(calculation_time, draw_time, fps);
	}

	// Cleanup
	delete graphics_manager;

	system("pause");
	exit(EXIT_SUCCESS);
}