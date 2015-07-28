
#include "VoxelEngine.h"
#include "ScriptEngine.h"
#include "Window.h"
#include "ConfigReader.h"

#include <SGL/SGL.h>
#include <SGL/Util/Exception.h>

#include <iostream>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#define VERSION "1.0"

using namespace engine;
using namespace engine::script;
using namespace boost;

/* Prototypes */

static void scriptError(const std::string &what);

static void registerUICallbacks(gui::Window* window);

/* UI callbacks */

static void onKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
static void onMouseMove(GLFWwindow*, double x, double y);

/* Globals */

static ScriptEngine* g_ScriptEngine;

int main(int argc, char *argv[])
{
	program_options::options_description desc("Options");
	desc.add_options()
		("help,h", "View command line options")
		("script,s", program_options::value<std::string>()->default_value("main.lua"), "Specify lua script file to run")
		("config,c", program_options::value<std::string>()->default_value("config.json"), "Specify engine configuration file, in json format");

	program_options::variables_map vm;
	program_options::store(program_options::parse_command_line(argc, argv, desc), vm);

	std::string script = vm["script"].as<std::string>();
	std::string config = vm["config"].as<std::string>();

	// check the help options
	if (vm.count("help"))
	{
		std::cout << desc << std::endl;
		return 0;
	}

	// load optional config file
	if (filesystem::exists(config))
		ConfigReader::getSingleton().load(config);
	else
		std::cout << "Error cannot load configuration file. " << config << " does not exist\nUse --help or -h for help" << std::endl;

	// Initialize the scripting engine
	ScriptEngine& scriptEngine = ScriptEngine::getSingleton();
	scriptEngine.init();
	scriptEngine.setErrorCallback(scriptError);

	scriptEngine.addFunction("registerUICallbacks", registerUICallbacks);

	// set global
	g_ScriptEngine = &scriptEngine;

	// init GLFW
	if (!glfwInit())
	{
		std::cout << "failed to initialize GLFW" << std::endl;
		return -1;
	}

	// check if the script file exists
	if (filesystem::exists(script))
	{
		// run script
		scriptEngine.run(script);
	}
	else
	{
		std::cout << "Error cannot load script. " << script << " does not exist" << std::endl;
		std::cout << "use --help or -h for help" << std::endl;
		return 1;
	}


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

	g_ScriptEngine->callFunction("onKeyEvent", key, action);
}

void onMouseMove(GLFWwindow* window, double x, double y)
{
	g_ScriptEngine->callFunction("onMouseMove", x, y);
}

void scriptError(const std::string& what)
{
	std::cout << what << std::endl;
}