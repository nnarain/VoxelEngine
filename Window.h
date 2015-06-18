
#ifndef WINDOW_H
#define WINDOW_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>

namespace engine
{
	namespace gui
	{
		class Window
		{
		public:

			/**
				Create a window with a title and dimensions
			*/
			Window(const std::string& title, int w, int h);
			~Window();

			bool shouldClose();
			void pollEvents();

			void setMousePosition(float x, float y);

			void swapBuffers();

			GLFWwindow *getWindow();

			int getWidth();
			int getHeight();

		private:

			GLFWwindow* _window;
			int _width;
			int _height;

		};
	}
}

#endif
