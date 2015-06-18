
#include "VoxelEngine.h"

#include <iostream>

using namespace engine;

VoxelEngine::VoxelEngine()
{
}

void VoxelEngine::render()
{
	std::set<ChunkManager*>::iterator iter;
	for (iter = _chunkManagers.begin(); iter != _chunkManagers.end(); ++iter)
	{
		ChunkManager* manager = (*iter);
		manager->render();
	}
}

void VoxelEngine::addChunkManager(ChunkManager* manager)
{
	_chunkManagers.insert(manager);
}

VoxelEngine* VoxelEngine::getEngine()
{
	static VoxelEngine engine;
	return &engine;
}

VoxelEngine::~VoxelEngine()
{
}

