
#include "VoxelEngine.h"
#include "ScriptEngine.h"
#include "Window.h"

#include <SGL/SGL.h>
#include <SGL/Util/Exception.h>

#include <iostream>

#define VERSION "1.0"

#define ENABLE_SCRIPTING

using namespace engine;
using namespace engine::script;

/* Prototypes */

static void scriptError(const char *what);

static void registerUICallbacks(gui::Window* window);

/* UI callbacks */

static void onKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
static void onMouseMove(GLFWwindow*, double x, double y);

/* Globals */

static ScriptEngine* g_ScriptEngine;

int main(int argc, char *argv[])
{
	// check for valid number of arguments
	if (argc < 2)
	{
		std::cout << "Please provide a script to run" << std::endl;
		return -1;
	}

	// init GLFW
	if (!glfwInit())
	{
		std::cout << "failed to initialize GLFW" << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_OPENGL_PROFILE, 0);

	//

	//
#ifdef ENABLE_SCRIPTING
	char *scriptName = argv[1];

	// Initialize the scripting engine
	ScriptEngine scriptEngine;
	scriptEngine.init();
	scriptEngine.setErrorCallback(scriptError);

	scriptEngine.addFunction("registerUICallbacks", registerUICallbacks);

	g_ScriptEngine = &scriptEngine;

	// run the script
	scriptEngine.run(scriptName);
#endif

	// terminate glfw
	glfwTerminate();

	return 0;
}

void registerUICallbacks(gui::Window* window)
{
	GLFWwindow* gWindow = window->getWindow();

	glfwSetKeyCallback(gWindow, onKeyEvent);
	glfwSetCursorPosCallback(gWindow, onMouseMove);
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