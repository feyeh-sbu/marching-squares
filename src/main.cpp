#include "shader.h"
#include "marching_squares.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>
#include <string>
#include <vector>

/* global variables */
unsigned int g_window_width  = 800;
unsigned int g_window_height = 800;
std::string g_window_name = "Marching Squares";

unsigned int g_grid_spacing = 15;

/* function callback prototypes */
void process_input(GLFWwindow* window);

int main()
{
	/* initialize glfw */
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	/* create window and set its context as current */
	GLFWwindow* window = glfwCreateWindow(g_window_width,
										  g_window_height,
										  g_window_name.c_str(),
										  nullptr,
										  nullptr);
	if (!window)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}
	glfwMakeContextCurrent(window);

	/* set up glfw callback functions */


	/* load OpenGL function pointers */
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return EXIT_FAILURE;
	}


	/****************
	 * OPENGL STUFF *
	 ****************/
	MarchingSquares ms;

	/* render loop */
	glEnable(GL_POINT_SIZE);
	glViewport(0, 0, g_window_width, g_window_height);

	double wait_time = 0.1;
	double last_render_time = glfwGetTime();

	while (!glfwWindowShouldClose(window))
	{
		process_input(window);

		ms.compute_sample_points(glfwGetTime());
		ms.march_squares();
		ms.render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return EXIT_SUCCESS;
}

void process_input(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}