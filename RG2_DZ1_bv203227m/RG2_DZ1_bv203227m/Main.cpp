#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "ShaderClass.h"

int WIDTH = 800;
int HEIGHT = 600;

int iterations = 200;
double zoom = 100.0;
double offsetX = 0.0;
double offsetY = 0.0;

bool dragging = false;
double oldX, oldY;

Shader* shaderProgram;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void window_size_callback(GLFWwindow* window, int width, int height);

int main() {
	if (!glfwInit()) {
		std::cout << "GLFW initialization error" << std::endl;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Mandelbrot Set", NULL, NULL);
	if (!window) {
		std::cout << "Window creation failed" << std::endl;
		glfwTerminate();
	}

	glfwSetErrorCallback([](int e, const char* s) { std::cout << s << std::endl; });
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);

	glfwMakeContextCurrent(window);

	if (glewInit() != 0) {
		std::cout << "GLEW initialization error" << std::endl;
	}

	float vertices[] = {
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		 1.0f,  1.0f,

		 1.0f,  1.0f,
		-1.0f,  1.0f,
		-1.0f, -1.0f,
	};

	GLuint vao, vbo;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	shaderProgram = new Shader("mandelbrot.vert", "mandelbrot.frag");
	shaderProgram->Activate();
	glUniform2d(glGetUniformLocation(shaderProgram->ID, "screenSize"), (double)WIDTH, (double)HEIGHT);
	glUniform2d(glGetUniformLocation(shaderProgram->ID, "offset"), offsetX, offsetY);
	glUniform1d(glGetUniformLocation(shaderProgram->ID, "zoom"), zoom);
	glUniform1i(glGetUniformLocation(shaderProgram->ID, "iterations"), iterations);

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	shaderProgram->Delete();

	glfwDestroyWindow(window);
	glfwTerminate();
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// ESC exits the program
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, 1);
	}

	// Numpad 0 performs a reset
	if (key == GLFW_KEY_KP_0 && action == GLFW_PRESS) {
		iterations = 200;
		zoom = 100.0;
		offsetX = 0.0;
		offsetY = 0.0;
	}

	// Arrow keys pan the view
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
		offsetX += 20 / zoom;
	}
	else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
		offsetX -= 20 / zoom;
	}
	else if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
		offsetY -= 20 / zoom;
	}
	else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
		offsetY += 20 / zoom;
	}

	// Numpad * and / zoom in and out
	if (key == GLFW_KEY_KP_MULTIPLY && action == GLFW_PRESS) {
		zoom *= 2;
	}
	else if (key == GLFW_KEY_KP_DIVIDE && action == GLFW_PRESS) {
		zoom /= 2;
	}

	// Numpad + and - keys increase and decrease the number of iterations
	if (key == GLFW_KEY_KP_ADD && action == GLFW_PRESS) {
		iterations += 10;
	}
	else if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_PRESS) {
		(iterations > 10) ? iterations -= 10 : iterations = 1;
	}

	// Pass modified values to the shader program
	glUniform1i(glGetUniformLocation(shaderProgram->ID, "iterations"), iterations);
	glUniform1d(glGetUniformLocation(shaderProgram->ID, "zoom"), zoom);
	glUniform2d(glGetUniformLocation(shaderProgram->ID, "offset"), offsetX, offsetY);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		glfwGetCursorPos(window, &oldX, &oldY);
		dragging = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		dragging = false;
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	if (dragging) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		offsetX += (xpos - oldX) / zoom;
		offsetY += (oldY - ypos) / zoom;

		oldX = xpos;
		oldY = ypos;

		glUniform2d(glGetUniformLocation(shaderProgram->ID, "offset"), offsetX, offsetY);
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	if (yoffset != 0) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		double dx = (xpos - WIDTH / 2) / zoom - offsetX;
		double dy = (HEIGHT - ypos - HEIGHT / 2) / zoom - offsetY;
		offsetX = -dx;
		offsetY = -dy;
		if (yoffset < 0) {
			zoom /= 1.1;
		}
		else {
			zoom *= 1.1;
		}

		dx = (xpos - WIDTH / 2) / zoom;
		dy = (HEIGHT - ypos - HEIGHT / 2) / zoom;
		offsetX += dx;
		offsetY += dy;

		glUniform1d(glGetUniformLocation(shaderProgram->ID, "zoom"), zoom);
		glUniform2d(glGetUniformLocation(shaderProgram->ID, "offset"), offsetX, offsetY);
	}
}

void window_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	WIDTH = width;
	HEIGHT = height;
	glUniform2d(glGetUniformLocation(shaderProgram->ID, "screenSize"), (double)WIDTH, (double)HEIGHT);
}