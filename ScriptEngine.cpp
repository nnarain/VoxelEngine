
#include "ScriptEngine.h"

// exported classes
#include "VoxelEngine.h"
#include "Window.h"
#include "Timer.h"
#include "Noise.h"
#include "FPSCamera.h"
#include "CommandLine.h"

#include <SGL/Math/Vector3.h>

using namespace engine;
using namespace engine::script;

ScriptEngine::ScriptEngine() : _errorCallback(nullptr)
{
	_state = luaL_newstate();
}

void ScriptEngine::init()
{
	using namespace luabind;

	open(_state);

	luaL_openlibs(_state);

	// export classes
	module(_state)[

		class_<VoxelEngine>("Engine")
			.scope[
				def("getEngine", &VoxelEngine::getEngine)
			]
			.def("update",           &VoxelEngine::update)
			.def("render",           &VoxelEngine::render)
			.def("createWindow",     &VoxelEngine::createWindow)
			.def("getWindow",        &VoxelEngine::getWindow)
			.def("getCamera",        &VoxelEngine::getCamera)
			.def("addManager",       &VoxelEngine::addChunkManager)
			.def("updateCameraView", &VoxelEngine::updateCamera)
			.def("loadTexture",      &VoxelEngine::loadTexture)
			.def("loadAtlas",        &VoxelEngine::loadAtlas)
			.def("loadFont",         &VoxelEngine::loadFont)
			.def("setRenderMode",    &VoxelEngine::setRenderer)
			.def("setRenderOption",  &VoxelEngine::setRenderOption),

		class_<Block>("Block")
			.def_readonly("t", &Block::t)
			.def_readonly("x", &Block::x)
			.def_readonly("y", &Block::y)
			.def_readonly("z", &Block::z),

		class_<ChunkManager>("ChunkManager")
			.def(constructor<int, int, int, const char *>())
			.def(constructor<int, int, int, int, float, const char *>())
			.def("getBlock",       &ChunkManager::getBlock)
			.def("setBlock",       &ChunkManager::setBlock)
			.def("setLightSource", &ChunkManager::setLightSource)
			.def("removeLight",    &ChunkManager::removeLight)
			.def("setAtlasName",   &ChunkManager::setAtlasName)
			.def("getBlockX",      &ChunkManager::getBlockX)
			.def("getBlockY",      &ChunkManager::getBlockY)
			.def("getBlockZ",      &ChunkManager::getBlockZ)
			.def("setRenderDebug", &ChunkManager::setRenderDebug)
			.def("translate",      &ChunkManager::translate)
			.def("rotate",         &ChunkManager::rotate)
			.def("scale",          &ChunkManager::scale),

		class_<FPSCamera>("Camera")
			.def_readwrite("position",  &FPSCamera::position)
			.def_readwrite("direction", &FPSCamera::direction)
			.def_readwrite("right",     &FPSCamera::right),

		class_<gui::CommandLine>("CommandLine")
		.def(constructor<>())
		.def("append", &gui::CommandLine::append),

		class_<gui::Window>("Window")
			.def(constructor<const char*, int, int>())
			.def("shouldClose",      &gui::Window::shouldClose)
			.def("pollEvents",       &gui::Window::pollEvents)
			.def("swapBuffers",      &gui::Window::swapBuffers)
			.def("setMousePosition", &gui::Window::setMousePosition)
			.def("isKeyPressed",     &gui::Window::isKeyPressed)
			.def("isKeyReleased",    &gui::Window::isKeyReleased)
			.def("getCommandLine",   &gui::Window::getCommandLine)
			
			// key constants
			.scope[
				class_<gui::Window::Key>("Key")
					.enum_("key_constants")[
						value("A", GLFW_KEY_A),
						value("B", GLFW_KEY_B),
						value("C", GLFW_KEY_C),
						value("D", GLFW_KEY_D),
						value("E", GLFW_KEY_E),
						value("F", GLFW_KEY_F),
						value("G", GLFW_KEY_G),
						value("H", GLFW_KEY_H),
						value("I", GLFW_KEY_I),
						value("J", GLFW_KEY_J),
						value("K", GLFW_KEY_K),
						value("L", GLFW_KEY_L),
						value("M", GLFW_KEY_M),
						value("N", GLFW_KEY_N),
						value("O", GLFW_KEY_O),
						value("P", GLFW_KEY_P),
						value("Q", GLFW_KEY_Q),
						value("R", GLFW_KEY_R),
						value("S", GLFW_KEY_S),
						value("T", GLFW_KEY_T),
						value("U", GLFW_KEY_U),
						value("V", GLFW_KEY_V),
						value("W", GLFW_KEY_W),
						value("X", GLFW_KEY_X),
						value("Y", GLFW_KEY_Y),
						value("Z", GLFW_KEY_Z),

						value("NUM_0", GLFW_KEY_0),
						value("NUM_1", GLFW_KEY_1),
						value("NUM_2", GLFW_KEY_2),
						value("NUM_3", GLFW_KEY_3),
						value("NUM_4", GLFW_KEY_4),
						value("NUM_5", GLFW_KEY_5),
						value("NUM_6", GLFW_KEY_6),
						value("NUM_7", GLFW_KEY_7),
						value("NUM_8", GLFW_KEY_8),
						value("NUM_9", GLFW_KEY_9),

						value("SPACE",  GLFW_KEY_SPACE),
						value("ENTER",  GLFW_KEY_ENTER),
						value("LCTRL",  GLFW_KEY_LEFT_CONTROL),
						value("LSHIFT", GLFW_KEY_LEFT_SHIFT),

						value("ACTION_PRESS",   GLFW_PRESS),
						value("ACTION_RELEASE", GLFW_RELEASE)
					]
			],

		class_<noise::Noise>("Noise")
			.def(constructor<int, int>())
			.def("generate", &noise::Noise::generate)
			.def("at",       &noise::Noise::at),

		class_<Timer>("Timer")
			.def(constructor<>())
			.def("getElapsed", &Timer::getElapsed),

		class_<sgl::Vector3>("Vector3")
			.def(constructor<float, float, float>())
			.def_readwrite("x", &sgl::Vector3::x)
			.def_readwrite("y", &sgl::Vector3::y)
			.def_readwrite("z", &sgl::Vector3::z)

			.def(self + other<sgl::Vector3>())
			.def(self - other<sgl::Vector3>())
			.def(self * float())
			.def(self / float())
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
			VoxelEngine::getEngine()->getLogger().log(error);
			_errorCallback(error);
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