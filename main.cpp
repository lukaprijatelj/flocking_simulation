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

void print_stats_deprecated(double calculation_time, double draw_time, float fps) {
	cout << '\r' << "Avg loop time: " << fps << " [calculation: " << calculation_time << "s] [draw: " << draw_time << "s]           " << flush;
}


int main() {
	int flock_size = 1000;
	int step_size = 200;
	int steps = 10;
	float step_runtime = 5.0f;

	for (int i = 0; i < steps; i++) {
		Graphics_manager *graphics_manager = new(Graphics_manager);
		Flock flock = Flock(flock_size);
		Dimension window_dimension = graphics_manager->getDimensions();
		uint64_t loop_count = 0;


		// Generate all N number of birds and layout them on canvas.
		// Layout dimensions will be same as one of the window.
		flock.generate(window_dimension);
		flock.generateGrid(window_dimension, 50);


		// Start iterations
		clock_t begin_time = clock();
		while (graphics_manager->loop()) {
			loop_count++;

			//clock_t begin_draw_time = clock();
			graphics_manager->draw_birds(flock.birds, flock.number_of_birds);
			graphics_manager->swap_buffers();
			//clock_t end_draw_time = clock();

			// Recalculates positions for all birds in the flock.
			//clock_t begin_calculation_time = clock();
			flock.run();
			//clock_t end_calculation_time = clock();

			//double calculation_time = double(end_calculation_time - begin_calculation_time) / CLOCKS_PER_SEC;
			//double draw_time = double(end_draw_time - begin_draw_time) / CLOCKS_PER_SEC;
			float runtime = (float(clock() - begin_time) / CLOCKS_PER_SEC);
			if (runtime > step_runtime) {
				cout << flock_size << " " << 1.0f / (runtime / loop_count) << endl << flush;
				break;
			}
		}

		// Cleanup
		delete graphics_manager;

		flock_size += step_size;
	}

	system("pause");
	exit(EXIT_SUCCESS);
}