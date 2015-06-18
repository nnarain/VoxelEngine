
#include "ChunkManager.h"

using namespace engine;

ChunkManager::ChunkManager(int x, int y, int z) : ChunkManager(x, y, z, 16, 1)
{
}

ChunkManager::ChunkManager(int x, int y, int z, int chunkSize, float blockSize) :
	_blockX(x),
	_blockY(y),
	_blockZ(z),
	_chunkSize(chunkSize),
	_blockSize(blockSize),
	_rebuildsPerFrame(5)
{
	allocateChunks(chunkSize, blockSize);
}

void ChunkManager::update()
{


	rebuildChunks();
}

void ChunkManager::render()
{
	ChunkSet::iterator iter;
	for (iter = _chunkRenderSet.begin(); iter != _chunkRenderSet.end(); ++iter)
	{
		Chunk* chunk = (*iter);
		chunk->render();
	}
}


Block ChunkManager::getBlockFromWorldPosition(const sgl::Vector3& p)
{
	return getBlockFromWorldPosition(p.x, p.y, p.z);
}


Block ChunkManager::getBlockFromWorldPosition(float x, float y, float z)
{
	int blockRenderSize = _blockSize * 2;

	int blockX = (int)x / blockRenderSize;
	int blockY = (int)y / blockRenderSize;
	int blockZ = (int)z / blockRenderSize;

	return getBlock(blockX, blockY, blockZ);
}

Block ChunkManager::getBlock(int x, int y, int z)
{
	int chunkX = x / _chunkSize;
	int chunkY = y / _chunkSize;
	int chunkZ = z / _chunkSize;

	int blockX = x % _chunkSize;
	int blockY = y % _chunkSize;
	int blockZ = z % _chunkSize;

	Chunk& chunk = getChunk(chunkX, chunkY, chunkZ);

	return chunk.getBlock(blockX, blockY, blockZ);
}

void ChunkManager::setBlock(int x, int y, int z, int t)
{
	int chunkX = x / _chunkSize;
	int chunkY = y / _chunkSize;
	int chunkZ = z / _chunkSize;

	int blockX = x % _chunkSize;
	int blockY = y % _chunkSize;
	int blockZ = z % _chunkSize;

	Chunk& chunk = getChunk(chunkX, chunkY, chunkZ);
	chunk.setBlock(blockX, blockY, blockZ, t);
}

Chunk& ChunkManager::getChunk(int x, int y, int z)
{
	Chunk& chunk = *(_chunks[(x * _chunkSize) + (y * _chunkSize * _chunkSize) + z]);

	if (!chunk.hasLocation())
	{
		chunk.setLocation(x, y, z);
	}

	return chunk;
}

void ChunkManager::rebuildChunks()
{
	// nothing to rebuild
	if (_chunkRebuildSet.size() == 0) return;

	int rebuiltCount = 0;

	ChunkList rebuilt;

	ChunkSet::iterator iter;
	for (iter = _chunkRebuildSet.begin(); rebuiltCount < _rebuildsPerFrame && iter != _chunkRebuildSet.end(); ++iter)
	{
		// build the chunk
		Chunk* chunk = (*iter);
		chunk->build();

		// set it to be removed later
		rebuilt.push_back(chunk);

		// add to the render list
		_chunkRenderSet.insert(chunk);

		//
		rebuiltCount++;
	}

	// remove rebuilt chunks
	ChunkList::iterator rebuiltIter;
	for (rebuiltIter = rebuilt.begin(); rebuiltIter != rebuilt.end(); ++rebuiltIter)
	{
		_chunkRebuildSet.erase(*rebuiltIter);
	}
}

int ChunkManager::getBlockX() const
{
	return _blockX;
}

int ChunkManager::getBlockY() const
{
	return _blockY;
}

int ChunkManager::getBlockZ() const
{
	return _blockZ;
}

void ChunkManager::allocateChunks(int chunkSize, float blockSize)
{
	int xSize = _blockX / chunkSize;
	int ySize = _blockY / chunkSize;
	int zSize = _blockZ / chunkSize;

	int size;

	if (xSize >= ySize && xSize >= zSize) size = xSize;
	if (ySize >= xSize && ySize >= zSize) size = ySize;
	if (zSize >= ySize && zSize >= xSize) size = zSize;

	int chunksToAllocate = size * size * size;

	int i;
	for (i = 0; i < chunksToAllocate; ++i)
	{
		_chunks.push_back(new Chunk(chunkSize, blockSize));
	}
}

ChunkManager::~ChunkManager()
{
}
