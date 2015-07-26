
#include "VoxelEngine.h"
#include "DeferredRenderer.h"
#include "DebugDeferredRenderer.h"
#include "CommandLine.h"

#include <SGL/SGL.h>
#include <SGL/Util/Context.h>
#include <SGL/Util/PerspectiveCamera.h>

#include <glfw/glfw3.h>

#include <iostream>
#include <cassert>

#include <boost/filesystem.hpp>

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

	_textRenderer->begin();
	{
		_textRenderer->draw(_commandLine->getText(), true, false);
	}
	_textRenderer->end();
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

void VoxelEngine::init(const char * title, int width, int height)
{
	// create the window
	_window = std::make_unique<gui::Window>(title, width, height);
	
	// tell SGL the window dimensions
	Context::setViewPortDimension((float)width, (float)height);

	// initialize the opengl context
	initializeContext();

	// load default resources
	_resources.getFontManager().addFont("resources/DefaultFont.DDS", 16, 8, false);

	// initialize the command line
	_commandLine = std::make_unique<gui::CommandLine>();
	_commandLine->init();

	// the functions to the command line interface
	addCommandLineFunctions();

	// initialize the text renderer
	_textRenderer = std::make_unique<TextRenderer>();
	_textRenderer->init();

	// load optional config file
	if (boost::filesystem::exists("config.json"))
		_config.load("config.json");
}

void VoxelEngine::initializeContext()
{
	glfwMakeContextCurrent(_window->getWindow());
	sgl::init();

	allocateRenderers();

	// set to the default renderer
	setRenderer(0);
}

void VoxelEngine::addCommandLineFunctions()
{
	_commandLine->addCommand("setrendermode",
		[](gui::CommandLine::StringList& args)
		{
			if (args.size() < 1) return false;

			std::istringstream buffer(args[0]);

			int mode;
			buffer >> mode;

			VoxelEngine::getEngine()->setRenderer(mode);

			return true;
		}
	);

	_commandLine->addCommand("setrenderoption",
		[](gui::CommandLine::StringList& args)
		{
			if (args.size() < 2) return false;

			std::string key = args[0];
			std::string value = args[1];

			std::transform(key.begin(), key.end(), key.begin(), tolower);
			std::transform(value.begin(), value.end(), value.begin(), tolower);

			VoxelEngine::getEngine()->setRenderOption(key.c_str(), value.c_str());

			return true;
		}
	);
}

void VoxelEngine::allocateRenderers()
{
	_renderers.push_back(new DeferredRenderer());
	_renderers.push_back(new DebugDeferredRenderer());
}

void VoxelEngine::loadTexture(const char *textureName)
{
	_resources.getTextureManager().addTexture(textureName);
}

void VoxelEngine::loadAtlas(const char *atlasName)
{
	_resources.getTextureManager().addAtlas(atlasName);
}

void VoxelEngine::loadFont(const char *fontname, int cols, int rows, bool flip)
{
	_resources.getFontManager().addFont(fontname, (unsigned int)cols, (unsigned int)rows, flip);
}

IRenderer& VoxelEngine::getRenderer()
{
	return *(_renderer);
}

void VoxelEngine::setRenderer(unsigned int idx)
{
	if (idx >= _renderers.size()) return;

	_renderer = _renderers[idx];
	
	if (!_renderer->isInitialized())
		_renderer->init();
}

void VoxelEngine::setRenderOption(const char *key, const char *value)
{
	_renderer->setRenderOption(std::string(key), std::string(value));
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

gui::CommandLine* VoxelEngine::getCommandLine()
{
	return _commandLine.get();
}

ConfigReader& VoxelEngine::getConfig()
{
	return _config;
}

VoxelEngine* VoxelEngine::getEngine()
{
	static VoxelEngine engine;
	return &engine;
}

VoxelEngine::~VoxelEngine()
{
	for (IRenderer* renderer : _renderers)
		delete renderer;
}



