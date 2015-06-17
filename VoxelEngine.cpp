
#include "VoxelEngine.h"

#include <iostream>

using namespace engine;

VoxelEngine::VoxelEngine()
{
}

void VoxelEngine::render()
{
	
}

VoxelEngine* VoxelEngine::getEngine()
{
	static VoxelEngine engine;
	return &engine;
}

VoxelEngine::~VoxelEngine()
{
}

