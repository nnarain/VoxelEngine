
#ifndef WINDOW_H
#define WINDOW_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "CommandLine.h"

#include <SGL/Math/Vector2.h>

#include <string>

namespace engine
{
	namespace gui
	{
		class Window
		{
		public:

			// empty struct for lua binding
			struct Key{};

			/**
				Create a window with a title and dimensions
			*/
			Window(const std::string& title, int w, int h);
			~Window();

			void init();

			bool shouldClose();
			void pollEvents();

			bool isKeyPressed(int key);
			bool isKeyReleased(int key);

			void setMousePosition(float x, float y);
			sgl::Vector2 getMousePosition();

			void swapBuffers();

			GLFWwindow *getWindow();

			int getWidth();
			int getHeight();

			CommandLine* getCommandLine();

		private:

			GLFWwindow* _window;
			int _width;
			int _height;

			CommandLine _commandLine;

		};
	}
}

#endif
