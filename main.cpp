// Set to false and recompile to remove graphics stuff 
#define RUN_WITH_GRAPHICS false

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include "bird.h"
#include "Vector.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "Flock.h"
#include <mpi.h>
#if RUN_WITH_GRAPHICS == true
#include "graphics_manager.h"
#endif

#define BUFSIZ 2000

using namespace std;

void print_stats_deprecated(double calculation_time, double draw_time, float fps) {
	cout << '\r' << "Avg loop time: " << fps << " [calculation: " << calculation_time << "s] [draw: " << draw_time << "s]           " << flush;
}


bool handleErr(int err) {
	if (err != MPI_SUCCESS) {
		char error_string[BUFSIZ];
		int length_of_error_string;

		int my_rank;
		MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

		MPI_Error_string(err, error_string, &length_of_error_string);
		fprintf(stderr, "MPI Error (%d): %s\n", my_rank, error_string);
		exit(EXIT_SUCCESS);
	}
	return false;
}

int main(int argc, char *argv[]) {
	// Initialize MPI
	MPI_Init(&argc, &argv);

	// Prepare MPI variables
	int my_rank; 
	int num_of_processes; 
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &num_of_processes);

	// Runtime variables 
	long loop_couner = 0;			// Loop counter
	long iteration_count = 3000;	// Number of iterations to run
	float max_runtime = 100.0f;			// The max number of seconds the program is allowed to run

	// Prepare local variables
	int flock_size = 1000;												// Flock size
	int bird_num_of_floats = 4;											// Number of floats needed to descrivbe a bird
	int calculated_birds_per_process = flock_size / num_of_processes;	// stevilo pticev na processor
	int num_of_additional_birds = flock_size % num_of_processes;		// stevilo processorjev ki bo moralo poracunati dodatnega ptica
	Dimension window_dimension = Dimension(1280, 720);					// Window dimensions
	
	// Prepare flock with grid
	Flock flock = Flock(flock_size);
	flock.generateGrid(window_dimension, 50);

#if RUN_WITH_GRAPHICS == true
	Graphics_manager *graphics_manager = NULL;
#endif
	
	// Prepare return data tables
	int calculated_birds_table_size = (calculated_birds_per_process + 1) * bird_num_of_floats;
	float *calculated_bird_data = new float[calculated_birds_table_size];

	// Prepare table of bird data nums
	int table_size = flock_size * bird_num_of_floats;
	float *bird_data = new float[table_size];

	printf("Hello");

	// Generate and randomize bird positions
	if (my_rank == 0) {
#if RUN_WITH_GRAPHICS == true
			graphics_manager = new(Graphics_manager);
#endif
		flock.generate(window_dimension);
	}

	// Measue start time
	clock_t begin_time = clock();

	// Begin iterations. Run iteration count loops. Or at most max_runtime seconds.
	while ( (loop_couner < iteration_count) && ( (float(clock() - begin_time) / CLOCKS_PER_SEC) < max_runtime) ) {
		loop_couner++;
		// Prepare bird data for sending (we send a table of floats)
		if (my_rank == 0) {
			for (int i = 0, j = 0; i < flock_size; i++) {
				bird_data[j++] = flock.birds[i]->position.x;
				bird_data[j++] = flock.birds[i]->position.y;
				bird_data[j++] = flock.birds[i]->velocity.x;
				bird_data[j++] = flock.birds[i]->velocity.y;
			}
		}
		// Distribute all birds
		int err = MPI_Bcast(bird_data, table_size, MPI_FLOAT, 0, MPI_COMM_WORLD);
		handleErr(err);
		// Convert table of floats back to a flock
		if (my_rank != 0) {
			flock.generateFromNumberTable(window_dimension, bird_data);
		}
		// Distribute birds in GRID
		flock.distributeBirds();
		// Calculate new bird positions
		int num_of_calculated = flock.run(my_rank, num_of_processes);

		if (my_rank == 0) {
			// Receive new bird positions from all processors
			for (int i = 1; i < num_of_processes; i++) {
				err = MPI_Recv(calculated_bird_data, calculated_birds_table_size, MPI_FLOAT, i, i, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
				handleErr(err);
				// Update bird positions in master flock
				int num_of_valid_birds = (i < num_of_additional_birds) ? calculated_birds_per_process + 1 : calculated_birds_per_process;
				for (int j = 0, k = i, l = 0; j < num_of_valid_birds; j++, k += num_of_processes) {
					flock.birds[k]->position.x = calculated_bird_data[l++];
					flock.birds[k]->position.y = calculated_bird_data[l++];
					flock.birds[k]->velocity.x = calculated_bird_data[l++];
					flock.birds[k]->velocity.y = calculated_bird_data[l++];
				}
			}
		}
		else {
			// Prepare bird data for sending (we send a table of floats)
			for (int i = 0, j = my_rank, k = 0; i < num_of_calculated; i++, j += num_of_processes) {
				calculated_bird_data[k++] = flock.birds[j]->position.x;
				calculated_bird_data[k++] = flock.birds[j]->position.y;
				calculated_bird_data[k++] = flock.birds[j]->velocity.x;
				calculated_bird_data[k++] = flock.birds[j]->velocity.y;
			}
			// Send new positions back to master
			err = MPI_Send(calculated_bird_data, calculated_birds_table_size, MPI_FLOAT, 0, my_rank, MPI_COMM_WORLD);
			handleErr(err);
		}
#if RUN_WITH_GRAPHICS == true
		if (my_rank == 0) {
			graphics_manager->draw_birds(flock.birds, flock.number_of_birds);
			graphics_manager->swap_buffers();
		}
#endif
	}

	if (my_rank == 0) {
#if RUN_WITH_GRAPHICS == true
		delete graphics_manager;
#endif
		// Report
		float avg_fps = (1.0f / (float(clock() - begin_time) / CLOCKS_PER_SEC / loop_couner));
		printf("Avg FPS: %f\n", avg_fps);
		fflush(stdout);
		
		int i;
		cin >> i;
	}

	// Clear MPI
	MPI_Finalize();
}