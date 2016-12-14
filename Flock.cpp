#include "Flock.h"
#include <time.h>
#include <omp.h>


Flock::Flock()
{
	omp_set_num_threads(64);
}

Flock::~Flock()
{
}

void Flock::generate(Dimension windowDimension) {
	birds = new Bird*[number_of_birds];
	srand(time(NULL));

	// Layout birds on the canvas
	for (int i = 0; i < number_of_birds; i++) {
		birds[i] = new Bird(windowDimension);

		float x = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / windowDimension.width));
		float y = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / windowDimension.height));

		/*birds[i]->position.x = x - windowDimension.width / 2;
		birds[i]->position.y = y - windowDimension.height / 2;*/

		Vector velocityV = Vector();
		velocityV.x = rand();
		velocityV.y = rand();
		velocityV.normalize(birds[i]->MAX_SPEED);
		birds[i]->velocity = velocityV;
		birds[i]->rotate();

		//birds[i]->report();
	}
}

void Flock::run() {
	
//#pragma offload target (mic) inout(birds:length(number_of_birds)) inout(birds:length(number_of_birds))
	#pragma omp parallel for
	for (int i = 0; i < number_of_birds; i++) {
		birds[i]->run(birds, number_of_birds);
		//printf("[%d]", omp_get_num_threads());
	}
}