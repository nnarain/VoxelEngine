
#include "VoxelEngine.h"

#include <SGL/SGL.h>
#include <SGL/Util/Context.h>
#include <SGL/Util/PerspectiveCamera.h>

#include <glfw/glfw3.h>

#include <iostream>
#include <cassert>

using namespace engine;
using namespace sgl;

VoxelEngine::VoxelEngine() : _camera(Vector3(10, 45, 10)), _logger("engine.log")
{
}

void VoxelEngine::update()
{
	if (_updateChunks)
	{
		updateChunkManagersVisibility(_camera.getFrustum());
		_updateChunks = false;
	}

	std::set<ChunkManager*>::iterator iter;
	for (iter = _chunkManagers.begin(); iter != _chunkManagers.end(); ++iter)
	{
		ChunkManager* manager = (*iter);
		manager->update();
	}
}

void VoxelEngine::render()
{
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

void VoxelEngine::updateChunkManagersVisibility(Frustum& frustum)
{
	std::set<ChunkManager*>::iterator iter;
	for (iter = _chunkManagers.begin(); iter != _chunkManagers.end(); ++iter)
	{
		ChunkManager* manager = *iter;
		manager->updateVisiblityList(frustum);
	}
}

void VoxelEngine::updateCamera(float delta)
{
	static Vector3 cameraPosition;
	static Vector3 cameraDirection;

	Vector2 mousePos = _window->getMousePosition();
	_window->setMousePosition((float)_window->getWidth() / 2, (float)_window->getHeight() / 2);

	_camera.updateLookDirection(mousePos.x, mousePos.y, delta);
	_camera.update();

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
	// create the window
	_window = std::make_unique<gui::Window>(title, width, height);
	
	// tell SGL the window dimensions
	Context::setViewPortDimension((float)width, (float)height);

	// initialize the opengl context
	initializeContext();
}

void VoxelEngine::initializeContext()
{
	glfwMakeContextCurrent(_window->getWindow());
	sgl::init();

	_renderer = std::make_unique<Renderer>();
	_renderer->init();

	_debugRenderer = std::make_unique<DebugRenderer>();
	_debugRenderer->init();
}

void VoxelEngine::loadTexture(const char *textureName)
{
	_resources.getTextureManager().addTexture(textureName);
}

void VoxelEngine::loadAtlas(const char *atlasName)
{
	_resources.getTextureManager().addAtlas(atlasName);
}

Renderer& VoxelEngine::getRenderer()
{
	return *(_renderer.get());
}

sgl::DebugRenderer& VoxelEngine::getDebugRenderer()
{
	return *(_debugRenderer.get());
}

ResourceManager& VoxelEngine::getResources()
{
	return _resources;
}

gui::Window* VoxelEngine::getWindow()
{
	return _window.get();
}

FPSCamera* VoxelEngine::getCamera()
{
	return &_camera;
}

util::Logger& VoxelEngine::getLogger()
{
	return _logger;
}

VoxelEngine* VoxelEngine::getEngine()
{
	static VoxelEngine engine;
	return &engine;
}

VoxelEngine::~VoxelEngine()
{
}



