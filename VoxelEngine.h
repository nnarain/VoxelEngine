
#ifndef VOXELENGINE_H
#define VOXELENGINE_H

#include <vector>

namespace engine
{
	class VoxelEngine
	{
	public:

		~VoxelEngine();

		static VoxelEngine& getEngine();

	private:
		VoxelEngine();
		VoxelEngine(const VoxelEngine&);

	};
}

#endif
