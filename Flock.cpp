#include "Flock.h"



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
	printf("[%.2f", bird.position.x);
	printf(", %.2f", bird.position.y);

	// Velocity
	printf("] velocity: [%.2f", bird.velocity.x);
	printf(", %.2f", bird.velocity.y);

	// Rotation
	printf("] rotation: %.2f\n", bird.rotation);
}

void Flock::generate(Dimension windowDimension) {
	birds = (Bird *)calloc(number_of_birds, sizeof(Bird));
	srand(time(NULL));

	// Layout birds on the canvas
	for (int i = 0; i < number_of_birds; i++) {
		float x = float(rand()) / RAND_MAX * windowDimension.width;
		float y = float(rand()) / RAND_MAX * windowDimension.height;

		birds[i].position.x = x - windowDimension.width / 2;
		birds[i].position.y = y - windowDimension.height / 2;

		birds[i].rotation = 0.0f;

		Vector velocityV = Vector();
		velocityV.x = rand();
		velocityV.y = rand();

		// Normalize
		float length = sqrt(velocityV.x * velocityV.x + velocityV.y * velocityV.y);

		if (length != 0) {
			velocityV.x = velocityV.x / length;
			velocityV.y = velocityV.y / length;

			velocityV.x = velocityV.x * MAX_SPEED;
			velocityV.y = velocityV.y * MAX_SPEED;
		}

		birds[i].velocity = velocityV;

		reportBird(birds[i]);
	}
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
	ret = clGetDeviceIDs(platform_id[1], CL_DEVICE_TYPE_GPU, 10, device_id, &ret_num_devices);  // (izbrana platforma, tip naprave, koliko naprav nas zanima kazalec na naprave, dejansko "stevilo naprav)

	// Kontekst
	context = clCreateContext(NULL, 1, &device_id[0], NULL, NULL, &ret);
	// (kontekst: vkljuèene platforme - NULL je privzeta, "stevilo naprav, kazalci na naprave, kazalec na call-back funkcijo v primeru napake, dodatni parametri funkcije, "stevilka napake)

	// Ukazna vrsta
	command_queue = clCreateCommandQueue(context, device_id[0], 0, &ret); // (kontekst, naprava, INORDER/OUTOFORDER, napake)
}

void Flock::buildKernel() {

	/* Alokacija pomnilnika na napravi */
	flock_obj = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, number_of_birds * sizeof(Bird), birds, &ret); // (kontekst, naèin, koliko, lokacija na hostu, napaka)

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
	clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&flock_obj); // (šèepec, številka argumenta, velikost podatkov, kazalec na podatke)
	clSetKernelArg(kernel, 1, sizeof(int), (void *)&number_of_birds); // (šèepec, številka argumenta, velikost podatkov, kazalec na podatke)

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
	//local_item_size[1] = WORKGROUP_SIZE;
	global_item_size[0] = closest_power_of_2;
	//global_item_size[1] = closest_power_of_2;
}

void Flock::run() {

	// šèepec: zagon
	ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, global_item_size, local_item_size, 0, NULL, NULL);
	// (vrsta, šèepec, dimenzionalnost, mora biti NULL, kazalec na število vseh niti, kazalec na lokalno število niti, dogodki, ki se morajo zgoditi pred klicem)

	// Kopiranje rezultatov
	ret = clEnqueueReadBuffer(command_queue, flock_obj, CL_TRUE, 0, number_of_birds * sizeof(Bird), birds, 0, NULL, NULL);
	// (branje v pomnilnik iz naparave, 0 = offset, zadnji trije - dogodki, ki se morajo zgoditi prej)



	/* Cleaning (if you release mem_object, then data will no longer stay on GPU when next round is executed. But we want data to STAY on GPU, so that we won't have to copy it again.) */
	//ret = clFlush(command_queue);
	//ret = clFinish(command_queue);
	//ret = clReleaseKernel(kernel);
	//ret = clReleaseProgram(program);
	//ret = clReleaseMemObject(image_obj);
	//ret = clReleaseCommandQueue(command_queue);
	//ret = clReleaseContext(context);
}