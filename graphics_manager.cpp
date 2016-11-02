#include "graphics_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

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

	if (!glfwInit()) {
		fprintf(stderr, "ERROR: Could not start GLFW3\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(WINDOW_SIZE_X, WINDOW_SIZE_Y, WINDOW_NAME, NULL, NULL);
	if (!window) {
		fprintf(stderr, "ERROR: Could not create window.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);

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

	float points[] = {
		0.0f,  0.5f,  0.0f,
		0.5f, -0.5f,  0.0f,
		-0.5f, -0.5f,  0.0f
	};
	vbo = 0; // Virtual Buffer Array (VBO)
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), points, GL_STATIC_DRAW);

	vao = 0; // Vertex Array Object (VAO)
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	const char* vertex_shader1 =
		"#version 400\n"
		"in vec3 vp;"
		"void main() {"
		"  gl_Position = vec4(vp, 1.0);"
		"}";

	const char* fragment_shader1 =
		"#version 400\n"
		"out vec4 frag_colour;"
		"void main() {"
		"  frag_colour = vec4(0.5, 0.0, 0.5, 1.0);"
		"}";

	// Compile vertex shader
	vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader1, NULL);
	glCompileShader(vs);

	// Compile fragment shader
	fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader1, NULL);
	glCompileShader(fs);

	// Create program from shaders
	sp = glCreateProgram();
	glAttachShader(sp, fs);
	glAttachShader(sp, vs);
	glLinkProgram(sp);
}

// Destructor
Graphics_manager::~Graphics_manager() {
	if (window != NULL)
		glfwDestroyWindow(window);
	glfwTerminate();
}


void Graphics_manager::draw_bird(Bird *bird) {
	// wipe the drawing surface clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(sp);
	glBindVertexArray(vao);
	// draw points 0-3 from the currently bound VAO with current in-use shader
	glDrawArrays(GL_TRIANGLES, 0, 3);
	return;
}


bool Graphics_manager::loop() {
	if (glfwWindowShouldClose(window))
		return false;

	draw_bird(NULL);

	// Perform one loop
	glfwSwapBuffers(window);
	glfwPollEvents();
	return true;
}
