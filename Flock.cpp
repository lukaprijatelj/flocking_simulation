#include "Flock.h"


struct calculateParameters
{
	Bird **birdArray;
	Bird *currentBird;
	int n;
	bool *threadRunning;
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

void Flock::initThreads()
{
	for (int i = 0; i < NUMBER_OF_THREADS; i++) {
		threadRunning[i] = false;
	}
}

void *runCurrenBird(void *arg)
{
	// Get arguments as struct
	struct calculateParameters *input = (struct calculateParameters*)arg;

	Bird currentBird = *input->currentBird;

	// Calculate new position
	currentBird.calculate(input->birdArray, input->n);

	// Move bird
	currentBird.update();

	// Rotate bird
	currentBird.rotate();

	// Check for borders
	currentBird.borders();

	// Set threadRunning to false
	input->threadRunning[input->threadIndex] = false;

	// TODO: A je potrebno to tudi uporabiti? 
	// pthread_exit(0);

	return NULL;
}

void Flock::run() {
	int processedBirds = 0;
	threads = new pthread_t[NUMBER_OF_THREADS];

	while (processedBirds < number_of_birds)
	{
		for (int i = 0; i < NUMBER_OF_THREADS; i++) 
		{
			if (processedBirds == number_of_birds)
				break;

			if (threadRunning[i] == true)
				continue;

			Bird currentBird = *birds[processedBirds];

			struct calculateParameters threadParams;
			threadParams.birdArray = birds;
			threadParams.n = number_of_birds;
			threadParams.threadIndex = i;
			threadParams.threadRunning = threadRunning;
			threadParams.currentBird = &currentBird;

			int returnCode = pthread_create(&threads[i], NULL, runCurrenBird, (void *)&threadParams);

			if (returnCode != 0) 
			{
				printf("ERROR; Return code from pthread_create is %d\n", returnCode);
				exit(-1);
			}
			else 
			{
				threadRunning[i] = true;
				processedBirds++;
			}			
		}
	}

	for (int i = 0; i < NUMBER_OF_THREADS; i++)
	{
		int errcode = pthread_join(threads[i], NULL);

		if (errcode != 0)
		{
			fprintf(stderr, "Error joining thread %d\n", errcode);
			exit(-1);
		}
	}

	free(threads);
}