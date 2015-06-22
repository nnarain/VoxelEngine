
#ifndef VOXELENGINE_H
#define VOXELENGINE_H

#include "ChunkManager.h"
#include "FPSCamera.h"
#include "Window.h"
#include "Renderer.h"

#include <set>

namespace engine
{
	class VoxelEngine
	{
	public:

		~VoxelEngine();

		void render();

		void addChunkManager(ChunkManager* manager);

		void updateCamera(float delta);

		void createWindow(const char * title, int width, int height);

		void loadTexture(const char *textureName);

		FPSCamera* getCamera();

		gui::Window* getWindow();

		Renderer& getRenderer();

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
		gui::Window* _window;

		//
		Renderer* _renderer;

		//
		FPSCamera _camera;

		/* private functions */

		void updateChunkManagers(FPSCamera& camera);

		void initializeContext();

	};
}

#endif
