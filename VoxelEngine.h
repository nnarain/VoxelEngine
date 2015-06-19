
#ifndef VOXELENGINE_H
#define VOXELENGINE_H

#include "ChunkManager.h"
#include "FPSCamera.h"
#include "Window.h"

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

		FPSCamera* getCamera();

		gui::Window* getWindow();

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
		FPSCamera _camera;

		/* private functions */

		void updateChunkManagers(sgl::Frustum& frustum);

		void initializeContext();

	};
}

#endif
