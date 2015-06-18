
#ifndef VOXELENGINE_H
#define VOXELENGINE_H

#include "ChunkManager.h"

#include <set>

namespace engine
{
	class VoxelEngine
	{
	public:

		~VoxelEngine();

		void render();

		void addChunkManager(ChunkManager* manager);

		static VoxelEngine* getEngine();

	private:
		/* Block creation of an engine from outside this class */
		VoxelEngine();
		VoxelEngine(const VoxelEngine&);

		/* private functions */

		std::set<ChunkManager*> _chunkManagers;

	};
}

#endif
