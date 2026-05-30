#include <iostream>
#include "Window.h"

Window::Window(int width, int height, const char* title) : width(width), height(height), title(title) {
	createWindow();
}

void Window::createWindow() {
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (window == NULL) {
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}

	glfwMakeContextCurrent(window);
}

void Window::pollEvents() {
	glfwPollEvents();
}

void Window::update() {
	glfwSwapBuffers(window);
}

int Window::getWidth() {
	return width;
}

int Window::getHeight() {
	return height;
}

GLFWwindow* Window::getWindow() {
	return window;
}

bool Window::shouldClose() {
	return glfwWindowShouldClose(window);
}

void Window::terminate() {
	glfwTerminate();
}

bool Window::isKeyPressed(unsigned int key) {
	return glfwGetKey(window, key) == GLFW_PRESS;
}