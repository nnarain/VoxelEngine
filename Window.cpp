
#include "Window.h"

using namespace engine;
using namespace engine::gui;

Window::Window(const std::string& title, int w, int h) : _width(w), _height(h), _window(NULL)
{
	_window = glfwCreateWindow(w, h, title.c_str(), NULL, NULL);

	if (!_window)
	{
		glfwTerminate();
		exit(1);
	}
}

bool Window::shouldClose()
{
	return glfwWindowShouldClose(_window) > 0;
}

void Window::pollEvents()
{
	glfwPollEvents();
}

void Window::setMousePosition(float x, float y)
{
	glfwSetCursorPos(_window, x, y);
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
}


