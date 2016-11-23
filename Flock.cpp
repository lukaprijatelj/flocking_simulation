#include "Flock.h"


struct calculateParameters
{
	Bird **birdArray;
	int n;
	int threadIndex;
};

Flock::Flock()
{
	initThreads();
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

		birds[i]->position.x = x - windowDimension.width / 2;
		birds[i]->position.y = y - windowDimension.height / 2;

		Vector velocityV = Vector();
		velocityV.x = rand();
		velocityV.y = rand();
		velocityV.normalize(birds[i]->MAX_SPEED);
		birds[i]->velocity = velocityV;
		birds[i]->rotate();

		birds[i]->report();
	}
}

void Flock::initThreads() {
	for (int i = 0; i < NUMBER_OF_THREADS; i++) {
		threadRunning[i] = false;
	}
}

void *runCurrenBird(void *arg)
{
	// Get arguments as struct
	struct calculateParameters *input = (struct calculateParameters*)arg;

	int num = input->n / NUMBER_OF_THREADS;
	int start = input->threadIndex * num;

	// Iterate over threads birds
	for (int i = start; i < start + num; i++) {
		//std::cout << input->threadIndex << " " << i << std::endl;
		Bird *currentBird = input->birdArray[i];

		// Calculate new position
		currentBird->calculate(input->birdArray, input->n);

		// Move bird
		currentBird->update();

		// Rotate bird
		currentBird->rotate();

		// Check for borders
		currentBird->borders();
	}

	pthread_exit(0);
	return NULL;
}

void Flock::run() {
	pthread_t threads[NUMBER_OF_THREADS];
	struct calculateParameters threadParams[NUMBER_OF_THREADS];

	for (int i = 0; i < NUMBER_OF_THREADS; i++) {
		
		threadParams[i].birdArray = birds;
		threadParams[i].n = number_of_birds;
		threadParams[i].threadIndex = i;

		int returnCode = pthread_create(&threads[i], NULL, runCurrenBird, (void *)&threadParams[i]);

		if (returnCode != 0) {
			printf("ERROR; Return code from pthread_create is %d\n", returnCode);
			exit(-1);
		}		
	}

	for (int i = 0; i < NUMBER_OF_THREADS; i++) {
		int errcode = pthread_join(threads[i], NULL);

		if (errcode != 0) {
			fprintf(stderr, "Error joining thread %d\n", errcode);
			exit(-1);
		}
	}
}