
#include "VoxelEngine.h"

#include <iostream>

using namespace engine;

VoxelEngine::VoxelEngine()
{
}

VoxelEngine& VoxelEngine::getEngine()
{
	static VoxelEngine engine;
	return engine;
}

VoxelEngine::~VoxelEngine()
{
}

