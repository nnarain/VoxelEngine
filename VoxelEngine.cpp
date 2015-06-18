
#include "VoxelEngine.h"

#include <SGL/SGL.h>
#include <SGL/Util/Context.h>

#include <glfw/glfw3.h>

#include <iostream>
#include <cassert>

using namespace engine;
using namespace sgl;

VoxelEngine::VoxelEngine() : _camera(Vector3(10, 10, 10)), _window(nullptr)
{
}

void VoxelEngine::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	std::set<ChunkManager*>::iterator iter;
	for (iter = _chunkManagers.begin(); iter != _chunkManagers.end(); ++iter)
	{
		ChunkManager* manager = (*iter);
		manager->render();
	}
}

void VoxelEngine::addChunkManager(ChunkManager* manager)
{
	_chunkManagers.insert(manager);
}

void VoxelEngine::updateCamera(float delta)
{
	static double lastX = -1;
	static double lastY = -1;

	GLFWwindow* gWindow = _window->getWindow();

	double x, y;
	glfwGetCursorPos(gWindow, &x, &y);
	glfwSetCursorPos(gWindow, _window->getWidth() / 2, _window->getHeight() / 2);

	_camera.updateLookDirection((float)x, (float)y, delta);
}

void VoxelEngine::createWindow(const char * title, int width, int height)
{
	assert(_window == nullptr);

	// create the window
	_window = new gui::Window(title, width, height);
	
	// tell SGL the window dimensions
	Context::setViewPortDimension((float)width, (float)height);

	// initialize the opengl context
	initializeContext();
}

void VoxelEngine::initializeContext()
{
	assert(_window != nullptr);

	glfwMakeContextCurrent(_window->getWindow());
	sgl::init();

	glClearColor(1, 1, 1, 1);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

gui::Window* VoxelEngine::getWindow()
{
	return _window;
}

FPSCamera* VoxelEngine::getCamera()
{
	return &_camera;
}

VoxelEngine* VoxelEngine::getEngine()
{
	static VoxelEngine engine;
	return &engine;
}

VoxelEngine::~VoxelEngine()
{
}

