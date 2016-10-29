#include "bird.h"
#include <stdio.h>


Bird::Bird() {
	for (int i = 0; i < 4; i++) {
		vector[i] = 0.0f;
	}
}

Bird::~Bird(){}

void Bird::report(){
	printf("Bird vector: ");
	printf("[%.2f", vector[0]);
	for (int i = 1; i < 4; i++) {
		printf(", %.2f", vector[i]);
	}
	printf("]\n");
}
