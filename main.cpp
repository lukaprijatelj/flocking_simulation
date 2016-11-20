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

using namespace std;


#define N 200 // Number of birds


void print_stats(float avg_loop_time) {
	cout << '\r' << "Avg loop time: " << avg_loop_time << flush;
}

int main() {
	Graphics_manager *graphics_manager = new(Graphics_manager);
	uint64_t loop_count = 0;

	float spacing = 0.2f;
	Bird **birds = new Bird*[N];
	
	int a = 0;
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < N / 5; j++) {
			birds[a] = new (Bird);

			float x = -1.0f + (spacing / 2.0f) + (spacing * i);
			float y = 1.0f - (spacing / 2.0f) - (spacing * j);

			birds[a]->setX(x);
			birds[a]->setY(y);

			Vector velocityV = Vector();
			velocityV.x = rand();
			velocityV.y = rand();
			velocityV.normalize(0.000001f);

			birds[a]->setVelX(velocityV.x);
			birds[a]->setVelY(velocityV.y);
			
			birds[a]->report();
			a++;
		}
	}
	

	// Start iterations
	clock_t begin_time = clock();
	while (graphics_manager->loop()) {
		loop_count++;
		graphics_manager->draw_birds(birds, N);
		graphics_manager->swap_buffers();

		
		// calculate next positions
		for (int i = 0; i < N; i++) {
			birds[i]->compute_new_position(birds, N);
		}

		// swap current values with next values
		for (int i = 0; i < N; i++) {
			birds[i]->swapNextValues();
		}

		print_stats(1.0 / (float(clock() - begin_time) / CLOCKS_PER_SEC / loop_count));
	}

	// Cleanup
	delete graphics_manager;

	system("pause");
	exit(EXIT_SUCCESS);
}