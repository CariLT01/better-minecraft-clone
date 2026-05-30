#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window {
public:
	Window(int width, int height, const char* title);
	~Window();

	void update();
	void pollEvents();
	
	int getWidth();
	int getHeight();

	GLFWwindow* getWindow();

	bool shouldClose();

	void terminate();

	bool isKeyPressed(unsigned int key);
private:

	GLFWwindow* window;

	void createWindow();

	int width;
	int height;
	const char* title;
	
	

};