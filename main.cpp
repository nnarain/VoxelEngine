
#include "ScriptEngine.h"
#include "Window.h"

#include <iostream>

#define VERSION "1.0"

using namespace engine;
using namespace engine::script;

/* Prototypes */

static void scriptError(const char *what);
static void initUserInput(ScriptEngine* scriptEngine);

static void registerUICallbacks(gui::Window* window);

/* UI callbacks */

static void onKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
static void onMouseMove(GLFWwindow*, double x, double y);

/* Globals */

static GLFWwindow* g_Window;
static ScriptEngine* g_ScriptEngine;

int main(int argc, char *argv[])
{
	// check for valid number of arguments
	if (argc < 2)
	{
		std::cout << "Please provide a script to run" << std::endl;
		return -1;
	}

	// script to run
	char *scriptName = argv[1];

	// Scripting Engine
	ScriptEngine scriptEngine;
	scriptEngine.init();
	scriptEngine.setErrorCallback(scriptError);

	scriptEngine.addFunction("registerUICallbacks", registerUICallbacks);

	g_ScriptEngine = &scriptEngine;

	// run the script
	scriptEngine.run(scriptName);

	return 0;
}

void registerUICallbacks(gui::Window* window)
{
	g_Window = window->getWindow();

	glfwSetKeyCallback(g_Window, onKeyEvent);
	glfwSetCursorPosCallback(g_Window, onMouseMove);
}

void onKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == 256) exit(0);

	g_ScriptEngine->callFunction("onKeyEvent", key);
}

void onMouseMove(GLFWwindow* window, double x, double y)
{
	g_ScriptEngine->callFunction("onMouseMove", x, y);
}

void scriptError(const char *what)
{
	std::cout << what << std::endl;
}