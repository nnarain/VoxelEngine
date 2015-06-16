
#include "Window.h"

#include <SGL/SGL.h>

using namespace engine;
using namespace engine::gui;

Window::Window(const std::string& title, int w, int h) : _width(w), _height(h), _window(NULL)
{
	glfwInit();
	_window = glfwCreateWindow(w, h, title.c_str(), NULL, NULL);

	if (!_window)
	{
		glfwTerminate();
		exit(1);
	}

	glfwMakeContextCurrent(_window);

	sgl::init();
}

bool Window::shouldClose()
{
	return glfwWindowShouldClose(_window) > 0;
}

void Window::pollEvents()
{
	glfwPollEvents();
}

void Window::swapBuffers()
{
	glfwSwapBuffers(_window);
}

GLFWwindow* Window::getWindow()
{
	return _window;
}

Window::~Window()
{
	glfwDestroyWindow(_window);
	glfwTerminate();
}


