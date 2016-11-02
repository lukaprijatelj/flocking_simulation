#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "bird.h"
#include "graphics_manager.h"

using namespace std;

int main() {
	Graphics_manager graphics_manager;

	while (graphics_manager.loop()) {

	}

	exit(EXIT_SUCCESS);
}