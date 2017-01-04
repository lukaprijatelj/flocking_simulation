#include "Flock.h"


struct calculateParameters
{
	Bird **birdArray;
	int n;
	int threadIndex;
};

Flock::Flock()
{
	
}

Flock::~Flock()
{
}

void Flock::reportBird(struct Bird bird)
{
	printf("Bird vector: ");

	// Position
	printf("[%.2f", bird.position[0]);
	printf(", %.2f", bird.position[1]);

	// Velocity
	printf("] velocity: [%.2f", bird.velocity[0]);
	printf(", %.2f", bird.velocity[1]);

	// Rotation
	printf("] rotation: %.2f\n", bird.rotation);
}

void Flock::generate(Dimension windowDimension) {
	birds = new Bird*[number_of_birds];
	srand(time(NULL));

	// Layout birds on the canvas
	for (int i = 0; i < number_of_birds; i++) {
		birds[i] = new Bird(windowDimension);

		float x = float(rand()) / RAND_MAX * windowDimension.width;
		float y = float(rand()) / RAND_MAX * windowDimension.height;

		birds[i]->position.x = x - windowDimension.width / 2;
		birds[i]->position.y = y - windowDimension.height / 2;

		Vector velocityV = Vector();
		velocityV.x = rand();
		velocityV.y = rand();
		velocityV.normalize(MAX_SPEED);
		birds[i]->velocity = velocityV;
		birds[i]->rotate();

		birds[i]->report();
	}
}



void *runCurrenBird(void *arg)
{
	// Get arguments as struct
	struct calculateParameters *input = (struct calculateParameters*)arg;

	int num = input->n / NUMBER_OF_THREADS;
	int start = input->threadIndex * num;

	

	pthread_exit(0);
	return NULL;
}


void Flock::initGPU() {
	/* Branje kernel-a (to je program za GPU) */
	FILE *fp;
	size_t source_size;

	fp = fopen("kernel.cl", "r");
	if (!fp)
	{
		fprintf(stderr, ":-(#\n");
		exit(1);
	}
	source_str = (char*)malloc(MAX_SOURCE_SIZE);
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	source_str[source_size] = '\0';
	fclose(fp);
	/* ------------------------------------- */

	/* Podatki o platformi */
	cl_platform_id	platform_id[10];
	cl_uint			ret_num_platforms;
	ret = clGetPlatformIDs(10, platform_id, &ret_num_platforms); // (max. "stevilo platform, kazalec na platforme, dejansko "stevilo platform)

	/* Podatki o napravi */
	cl_uint			ret_num_devices;

	// Delali bomo s platform_id[0] na GPU
	ret = clGetDeviceIDs(platform_id[0], CL_DEVICE_TYPE_GPU, 10, device_id, &ret_num_devices);  // (izbrana platforma, tip naprave, koliko naprav nas zanima kazalec na naprave, dejansko "stevilo naprav)

	// Kontekst
	context = clCreateContext(NULL, 1, &device_id[0], NULL, NULL, &ret);
	// (kontekst: vkljuèene platforme - NULL je privzeta, "stevilo naprav, kazalci na naprave, kazalec na call-back funkcijo v primeru napake, dodatni parametri funkcije, "stevilka napake)

	// Ukazna vrsta
	command_queue = clCreateCommandQueue(context, device_id[0], 0, &ret); // (kontekst, naprava, INORDER/OUTOFORDER, napake)
}

void Flock::buildKernel() {
	int *surface = (int*)calloc(1, sizeof(int));


	/* Alokacija pomnilnika na napravi */
	cl_mem image_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, IMAGE_WIDTH * IMAGE_HEIGHT * sizeof(int), NULL, &ret); // (kontekst, naèin, koliko, lokacija na hostu, napaka)
	cl_mem surface_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(int), NULL, &ret);

	/* Priprava programa */
	program = clCreateProgramWithSource(context, 1, (const char **)&source_str, NULL, &ret); // (kontekst, "stevilo kazalcev na kodo, kazalci na kodo, stringi so NULL terminated, napaka)													

	/* Prevajanje */
	ret = clBuildProgram(program, 1, &device_id[0], NULL, NULL, NULL); // (program, "stevilo naprav, lista naprav, opcije pri prevajanju, kazalec na funkcijo, uporabni"ski argumenti)

	/* Log */
	size_t build_log_len;
	char *build_log;
	ret = clGetProgramBuildInfo(program, device_id[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &build_log_len); // (program, "naprava, tip izpisa, maksimalna dolžina niza, kazalec na niz, dejanska dolžina niza)
	build_log = (char *)malloc(sizeof(char)*(build_log_len + 1));
	ret = clGetProgramBuildInfo(program, device_id[0], CL_PROGRAM_BUILD_LOG, build_log_len, build_log, NULL);
	printf("%s\n", build_log);
	free(build_log);

	// šèepec: priprava objekta
	kernel = clCreateKernel(program, "flockingSimulation", &ret); // (program, ime šèepca, napaka)

	// šèepec: argumenti
	clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&image_obj); // (šèepec, številka argumenta, velikost podatkov, kazalec na podatke)
	clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&surface_obj);
	clSetKernelArg(kernel, 2, sizeof(cl_int), (void *)&maxGray);

	// Calculate closest power of 2 needed for global item size
	unsigned int closest_power_of_2 = number_of_birds;
	closest_power_of_2--;
	closest_power_of_2 |= closest_power_of_2 >> 1;   // Divide by 2^k for consecutive doublings of k up to 32,
	closest_power_of_2 |= closest_power_of_2 >> 2;   // and then or the results.
	closest_power_of_2 |= closest_power_of_2 >> 4;
	closest_power_of_2 |= closest_power_of_2 >> 8;
	closest_power_of_2 |= closest_power_of_2 >> 16;
	closest_power_of_2++;

	// Delitev dela
	local_item_size[0] = WORKGROUP_SIZE;
	local_item_size[1] = WORKGROUP_SIZE;
	global_item_size[0] = closest_power_of_2;
	global_item_size[1] = closest_power_of_2;
}

void Flock::run() {
	
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














	// šèepec: zagon
	ret = clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, global_item_size, local_item_size, 0, NULL, NULL);
	// (vrsta, šèepec, dimenzionalnost, mora biti NULL, kazalec na število vseh niti, kazalec na lokalno število niti, dogodki, ki se morajo zgoditi pred klicem)

	// Kopiranje rezultatov
	ret = clEnqueueReadBuffer(command_queue, image_obj, CL_TRUE, 0, IMAGE_HEIGHT * IMAGE_WIDTH * sizeof(int), picture.image, 0, NULL, NULL);
	ret = clEnqueueReadBuffer(command_queue, surface_obj, CL_TRUE, 0, sizeof(int), surface, 0, NULL, NULL);
	// (branje v pomnilnik iz naparave, 0 = offset, zadnji trije - dogodki, ki se morajo zgoditi prej)



	// Cleaning (if you release mem_object, then data will no longer stay on GPU when next round is executed. But we want data to STAY on GPU, so that we won't have to copy it again.)
	//ret = clFlush(command_queue);
	//ret = clFinish(command_queue);
	//ret = clReleaseKernel(kernel);
	//ret = clReleaseProgram(program);
	//ret = clReleaseMemObject(image_obj);
	//ret = clReleaseMemObject(surface_obj);
	//ret = clReleaseCommandQueue(command_queue);
	//ret = clReleaseContext(context);
}