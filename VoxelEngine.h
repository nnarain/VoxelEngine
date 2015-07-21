
#ifndef VOXELENGINE_H
#define VOXELENGINE_H

#include "ChunkManager.h"
#include "FPSCamera.h"
#include "Window.h"
#include "DeferredRenderer.h"
#include "ResourceManager.h"
#include "Logger.h"

#include <SGL/Util/DebugRenderer.h>

#include <set>
#include <memory>

namespace engine
{
	class VoxelEngine
	{
	public:

		~VoxelEngine();
		
		void update();
		void render();

		void addChunkManager(ChunkManager* manager);

		void updateCamera(float delta);

		void createWindow(const char * title, int width, int height);

		void loadTexture(const char *textureName);
		void loadAtlas(const char *atlasName);

		FPSCamera* getCamera();

		gui::Window* getWindow();

		IRenderer& getRenderer();
		sgl::DebugRenderer& getDebugRenderer();

		ResourceManager& getResources();

		util::Logger& getLogger();

		/**
			Return the instance of the voxel engine
		*/
		static VoxelEngine* getEngine();

	private:
		/* Block creation of an engine from outside this class */
		VoxelEngine();
		VoxelEngine(const VoxelEngine&);

		//
		std::set<ChunkManager*> _chunkManagers;
		bool _updateChunks;

		//
		std::unique_ptr<gui::Window> _window;

		//
		IRenderer* _renderer;
		std::unique_ptr<sgl::DebugRenderer> _debugRenderer;

		// 
		ResourceManager _resources;

		//
		FPSCamera _camera;

		//
		util::Logger _logger;

	private:

		void updateChunkManagersVisibility(sgl::Frustum& frustum);

		void initializeContext();
	};
}

#endif
