
#include "VoxelEngine.h"

#include <SGL/SGL.h>
#include <SGL/Util/Context.h>

#include <glfw/glfw3.h>

#include <iostream>
#include <cassert>

using namespace engine;
using namespace sgl;

VoxelEngine::VoxelEngine() : _camera(Vector3(10, 10, 10)), _window(nullptr), _renderer(nullptr)
{
}

void VoxelEngine::render()
{
	if (_updateChunks)
	{
		updateChunkManagers(_camera);
		_updateChunks = false;
	}

	Matrix4 VP = _camera.getProjection() * _camera.getView();

	_renderer->begin();
	{
		std::set<ChunkManager*>::iterator iter;
		for (iter = _chunkManagers.begin(); iter != _chunkManagers.end(); ++iter)
		{
			ChunkManager* manager = (*iter);
			_renderer->render(*manager, VP);
		}
	}
	_renderer->end();
}

void VoxelEngine::addChunkManager(ChunkManager* manager)
{
	_chunkManagers.insert(manager);
}

void VoxelEngine::updateChunkManagers(FPSCamera& camera)
{
	std::set<ChunkManager*>::iterator iter;
	for (iter = _chunkManagers.begin(); iter != _chunkManagers.end(); ++iter)
	{
		ChunkManager* manager = *iter;
		manager->update(camera);
	}
}

void VoxelEngine::updateCamera(float delta)
{
	static Vector3 cameraPosition;
	static Vector3 cameraDirection;

	Vector2 mousePos = _window->getMousePosition();
	_window->setMousePosition(_window->getWidth() / 2, _window->getHeight() / 2);

	_camera.updateLookDirection(mousePos.x, mousePos.y, delta);
	_camera.update(delta);

	//
	Vector3& currentPosition = _camera.getPosition();
	Vector3& currentDirection = _camera.getDirection();

	// if the camera has moved
	if (currentPosition != cameraPosition || currentDirection != cameraDirection)
	{
		// update to the new position, direction and render distance
		cameraPosition = currentPosition;
		cameraDirection = currentDirection;

		//
		_updateChunks = true;
	}
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

	_renderer = new Renderer();
	_renderer->init();
}

void VoxelEngine::loadTexture(const char *textureName)
{
	_renderer->getTextureManager().addTexture(textureName);
}

void VoxelEngine::loadAtlas(const char *atlasName)
{
	_renderer->getTextureManager().addAtlas(atlasName);
}

Renderer& VoxelEngine::getRenderer()
{
	return *_renderer;
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
	if (_window != nullptr)   delete _window;
	if (_renderer != nullptr) delete _renderer;
}



