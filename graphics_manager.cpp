#include "graphics_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

//glcolor3...
//glVertex3f

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


// Constructor
Graphics_manager::Graphics_manager() {
	window = NULL;
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		exit(EXIT_FAILURE);
	window = glfwCreateWindow(WINDOW_SIZE_X, WINDOW_SIZE_Y, WINDOW_NAME, NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);

	// Load all OpenGL functions using the glfw loader function
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		fprintf(stderr, "ERROR: Failed to initialize OpenGL context.\n");
		if (window != NULL)
			glfwDestroyWindow(window);
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSwapInterval(1);
	glfwSetKeyCallback(window, key_callback);
}

// Destructor
Graphics_manager::~Graphics_manager() {
	if (window != NULL)
		glfwDestroyWindow(window);
	glfwTerminate();
}


void Graphics_manager::draw_birds(Bird **birds, int n) {
	float ratio;
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	ratio = width / (float)height;
	glViewport(0, 0, width, height);
	glClearColor(BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
	glMatrixMode(GL_MODELVIEW);
	
	// Draw the birds
	for (int i = 0; i < n; i++) {
		draw_bird(birds[i]);
	}

	return;
}

void Graphics_manager::draw_bird(Bird *bird) {
	glPushMatrix();
	glTranslatef(bird->getX(), bird->getY(), 0.0);
	glRotatef(bird->getRotation(), 0, 0, 1);

	glBegin(GL_TRIANGLES);
	glColor3f(bird->getColorR(), bird->getColorG(), bird->getColorB());
	glVertex3f(-0.02f, -0.02f, 0.00f);
	glVertex3f( 0.02f, -0.02f, 0.00f);
	glColor3f(1.f, 0.f, 1.f);
	glVertex3f( 0.00f, +0.03f, 0.00f);
	glEnd();
	
	glPopMatrix();
}


bool Graphics_manager::loop() {
	glfwPollEvents();
	if (glfwWindowShouldClose(window))
		return false;
	
	return true;
}

void Graphics_manager::swap_buffers() {
	glfwSwapBuffers(window);
}
