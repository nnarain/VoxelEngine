
#ifndef VOXELENGINE_H
#define VOXELENGINE_H

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
		VoxelEngine();
		VoxelEngine(const VoxelEngine&);

	};
}

#endif
