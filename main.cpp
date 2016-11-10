//#include "glad.h"
//#include "GLFW/glfw3.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include "bird.h"
#include "graphics_manager.h"

using namespace std;

#define N 30 // Number of birds

void print_stats(float avg_loop_time) {
	cout << '\r' << "Avg loop time: " << avg_loop_time << flush;
}

int main() {
	Graphics_manager *graphics_manager = new(Graphics_manager);
	uint64_t loop_count = 0;

	float spacing = 2.0f / N;
	Bird **birds = new Bird*[N];

	for (int i = 0; i < N; i++) {
		birds[i] = new(Bird);
		float x = -1.0f + (spacing / 2.0f) + (spacing * i);
		birds[i]->setX(x);
		birds[i]->setRotation((360.0f / N) * i);
		birds[i]->report();
	}


	// Start iterations
	clock_t begin_time = clock();
	while (graphics_manager->loop()) {
		loop_count++;
		graphics_manager->draw_birds(birds, N);
		graphics_manager->swap_buffers();

		for (int i = 0; i < N; i++) {
			birds[i]->setRotation(birds[i]->getRotation() + 1);
		}

		print_stats(1.0/(float(clock() - begin_time) / CLOCKS_PER_SEC / loop_count));
	}

	// Cleanup
	delete graphics_manager;

	system("pause");
	exit(EXIT_SUCCESS);
}