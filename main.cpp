#include "GLFW/glfw3.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "bird.h"

using namespace std;

// Error callback function that gets called in case of errors by glfw
static void error_callback(int error, const char* description) {
	fprintf(stderr, "GLWF error [%d]: %s\n", error, description);
}

// Key callback function that gest called by the open window
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE); // close on escape
	else if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE); // close on enter
}

int main() {
	GLFWwindow* window;

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	window = glfwCreateWindow(1280, 720, "Simple example", NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	while (!glfwWindowShouldClose(window)) {
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}