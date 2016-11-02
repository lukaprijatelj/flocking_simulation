#pragma once
#include "glad.h"
#include "GLFW/glfw3.h"
#include "bird.h"

#define WINDOW_SIZE_X 1280
#define WINDOW_SIZE_Y 720
#define WINDOW_NAME "Flocking Simulation"

class Graphics_manager {
	GLFWwindow* window;

	GLuint vbo; // Virtual Buffer Array (VBO)
	GLuint vao; // Vertex Array Object (VAO)
	GLuint vs;  // Vertex Shader (VS)
	GLuint fs;  // Fragment Shader (FS)
	GLuint sp;  // Shader Program (SP)

public:
	Graphics_manager();
	~Graphics_manager();

	void draw_bird(Bird*);
	bool loop();
};

