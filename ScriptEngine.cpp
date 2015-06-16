
#include "ScriptEngine.h"

// exported classes
#include "Window.h"
#include "Timer.h"

using namespace engine;
using namespace engine::script;

ScriptEngine::ScriptEngine() : _errorCallback(NULL)
{
	_state = luaL_newstate();
}

void ScriptEngine::init()
{
	using namespace luabind;

	open(_state);

	luaL_openlibs(_state);

	// export window class
	module(_state)[
		class_<gui::Window>("Window")
			.def(constructor<const char*, int, int>())
			.def("shouldClose", &gui::Window::shouldClose)
			.def("pollEvents",  &gui::Window::pollEvents)
			.def("swapBuffers", &gui::Window::swapBuffers),

		class_<Timer>("Timer")
			.def(constructor<>())
			.def("getElapsed", &Timer::getElapsed)
	];

}

void ScriptEngine::run(char *scriptName)
{
	// run the script file
	if (luaL_dofile(_state, scriptName) != 0)
	{
		// if an error occurs and there is a error callback function
		if (_errorCallback != NULL)
		{
			std::string error = getErrorString();
			_errorCallback(error.c_str());
		}
	}
}

std::string ScriptEngine::getErrorString()
{
	std::string error(lua_tostring(_state, -1));
	lua_pop(_state, 1);

	return error;
}

void ScriptEngine::setErrorCallback(ErrorCallback callback)
{
	_errorCallback = callback;
}

ScriptEngine::~ScriptEngine()
{
	lua_close(_state);
}