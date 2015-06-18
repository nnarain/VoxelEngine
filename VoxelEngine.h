
#ifndef VOXELENGINE_H
#define VOXELENGINE_H

#include "ChunkManager.h"

#include <vector>

namespace engine
{
	class VoxelEngine
	{
	public:

		~VoxelEngine();

		void render();

		static VoxelEngine* getEngine();

	private:
		/* Block creation of an engine from outside this class */
		VoxelEngine();
		VoxelEngine(const VoxelEngine&);

		/* private functions */

		std::vector<ChunkManager*> _chunkManagers;

	};
}

#endif
