
#include "Window.h"

namespace engine
{
	namespace gui
	{
		Window::Window(const std::string& title, int w, int h) : _width(w), _height(h), _window(NULL)
		{
			_window = glfwCreateWindow(w, h, title.c_str(), NULL, NULL);

			if (!_window)
			{
				glfwTerminate();
				exit(1);
			}

			glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		}

		bool Window::shouldClose()
		{
			return glfwWindowShouldClose(_window) > 0;
		}

		void Window::pollEvents()
		{
			glfwPollEvents();
		}

		bool Window::isKeyPressed(int key)
		{
			return glfwGetKey(_window, key) == GLFW_PRESS;
		}

		bool Window::isKeyReleased(int key)
		{
			return glfwGetKey(_window, key) == GLFW_RELEASE;
		}

		sgl::Vector2 Window::getMousePosition()
		{
			double x, y;
			glfwGetCursorPos(_window, &x, &y);

			float invY = (float)(-1) * ((float)y - (float)_height);

			return sgl::Vector2((float)x, (float)y);
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

		int Window::getWidth()
		{
			return _width;
		}

		int Window::getHeight()
		{
			return _height;
		}

		Window::~Window()
		{
			glfwDestroyWindow(_window);
		}

	}
}



