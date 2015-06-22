
#include "ChunkManager.h"

using namespace engine;
using namespace sgl;

ChunkManager::ChunkManager(int x, int y, int z) : ChunkManager(x, y, z, 16, 1)
{
}

ChunkManager::ChunkManager(int x, int y, int z, int blocksPerChunk, float blockSize) :
	_blockX(x),
	_blockY(y),
	_blockZ(z),
	_blocksPerChunk(blocksPerChunk),
	_blockSize(blockSize),
	_rebuildsPerFrame(5)
{
	allocateChunks(blocksPerChunk, blockSize);

	_modelMatrix.toTranslation(0, 0, 0);
}

void ChunkManager::update(FPSCamera& camera)
{
	updateVisiblityList(camera.getFrustum());

	rebuildChunks();
}

void ChunkManager::updateVisiblityList(sgl::Frustum& frustum)
{
	// clear the current chunks
	_chunkRenderSet.clear();

	//
	Vector3& nearCenter = frustum.getPoint(Frustum::PointId::NC);
	Vector3& farCenter  = frustum.getPoint(Frustum::PointId::FC);

	Vector3 center = (farCenter + nearCenter) / 2.0f;

	Chunk& chunk = getChunkFromWorldPosition(center);

	if (chunk.isSetup())
		_chunkRenderSet.insert(&chunk);
	else
		_chunkRebuildSet.insert(&chunk);

	//
	Vector3 loc = chunk.getLocation();
	int x = (int)loc.x;
	int y = (int)loc.y;
	int z = (int)loc.z;

	ChunkSet adjacent;

	int i, j, k;

	for (i = x - 1; i <= x + 1; ++i)
	{
		for (j = y - 1; j <= y + 1; ++j)
		{
			for (k = z - 1; k <= z + 1; ++k)
			{
				if (i < 0) i = 0;
				if (j < 0) j = 0;
				if (k < 0) k = 0;

				Chunk& chunk = getChunk(i, j, k);

				adjacent.insert(&chunk);
				_chunkVisibleSet.insert(&chunk);

				if (chunk.isSetup())
				{
					_chunkRenderSet.insert(&chunk);
				}
				else
				{
					_chunkRebuildSet.insert(&chunk);
				}
			}
		}
	}

	float frustumVolume = frustum.getVolume();
	fillFrustumVolume(frustumVolume, adjacent);
}

void ChunkManager::fillFrustumVolume(float volume, ChunkSet& chunks)
{
	ChunkSet adjacent;

	ChunkSet::iterator iter;
	for (iter = chunks.begin(); iter != chunks.end(); ++iter)
	{
		Chunk& chunk = *(*iter);

		Vector3 loc = chunk.getLocation();
		int x = (int)loc.x;
		int y = (int)loc.y;
		int z = (int)loc.z;

		// get the adjacent chunks

		int i, j, k;

		for (i = x - 1; i <= x + 1; ++i)
		{
			for (j = y - 1; j <= y + 1; ++j)
			{
				for (k = z - 1; k <= z + 1; ++k)
				{
					if (i < 0) i = 0;
					if (j < 0) j = 0;
					if (k < 0) k = 0;

					Chunk& chunk = getChunk(i, j, k);

					adjacent.insert(&chunk);
					_chunkVisibleSet.insert(&chunk);

					if (chunk.isSetup())
					{
						_chunkRenderSet.insert(&chunk);
					}
					else
					{
						_chunkRebuildSet.insert(&chunk);
					}
				}
			}
		}
	}

	if (_chunkVisibleSet.size() >= _chunks.size())
	{
		return;
	}
	else
	{
		float renderSize = (float)(_blocksPerChunk * _blockSize * 2);
		float chunkVolume = _chunkVisibleSet.size() * (float)(renderSize * renderSize * renderSize);

		if (chunkVolume < volume)
		{
			fillFrustumVolume(volume, adjacent);
		}
	}
}

void ChunkManager::render()
{
	ChunkSet::iterator iter;
	for (iter = _chunkRenderSet.begin(); iter != _chunkRenderSet.end(); ++iter)
	{
		Chunk* chunk = (*iter);
		
		if (chunk->shouldRender())
		{
			chunk->render();
		}
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

Chunk& ChunkManager::getChunkFromWorldPosition(const sgl::Vector3& pos)
{
	return getChunkFromWorldPosition(pos.x, pos.y, pos.z);
}

Chunk& ChunkManager::getChunkFromWorldPosition(float x, float y, float z)
{
	// world space size of a chunk
	float chunkRenderSize = (float)(_blocksPerChunk * _blockSize * 2);

	// get the x,y,z indices of the chunk at given location 

	int chunkX = (int)(x / chunkRenderSize);
	int chunkY = (int)(y / chunkRenderSize);
	int chunkZ = (int)(z / chunkRenderSize);

	//
	return getChunk(chunkX, chunkY, chunkZ);
}

Block ChunkManager::getBlock(int x, int y, int z)
{
	int chunkX = x / _blocksPerChunk;
	int chunkY = y / _blocksPerChunk;
	int chunkZ = z / _blocksPerChunk;

	int blockX = x % _blocksPerChunk;
	int blockY = y % _blocksPerChunk;
	int blockZ = z % _blocksPerChunk;

	Chunk& chunk = getChunk(chunkX, chunkY, chunkZ);

	return chunk.getBlock(blockX, blockY, blockZ);
}

void ChunkManager::setBlock(int x, int y, int z, int t)
{
	int chunkX = x / _blocksPerChunk;
	int chunkY = y / _blocksPerChunk;
	int chunkZ = z / _blocksPerChunk;

	int blockX = x % _blocksPerChunk;
	int blockY = y % _blocksPerChunk;
	int blockZ = z % _blocksPerChunk;

	Chunk& chunk = getChunk(chunkX, chunkY, chunkZ);
	chunk.setBlock(blockX, blockY, blockZ, t);
}

Chunk& ChunkManager::getChunk(int x, int y, int z)
{
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (z < 0) z = 0;

	Chunk& chunk = *(_chunks[(x * _size) + (y * _size * _size) + z]);

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

sgl::Matrix4& ChunkManager::getModelMatrix()
{
	return _modelMatrix;
}

void ChunkManager::setAtlasName(const std::string& name)
{
	_atlasName = name;
}

std::string ChunkManager::getAtlasName()
{
	return _atlasName;
}

void initChunk(Chunk* chunk)
{
	int i, k;
	for (i = 0; i < 16; ++i)
	{
		for (k = 0; k < 16; ++k)
		{
			chunk->setBlock(i, 0, k, 1);
		}
	}
}

void ChunkManager::allocateChunks(int chunkSize, float blockSize)
{
	int xSize = _blockX / chunkSize;
	int ySize = _blockY / chunkSize;
	int zSize = _blockZ / chunkSize;

	if (xSize >= ySize && xSize >= zSize) _size = xSize;
	if (ySize >= xSize && ySize >= zSize) _size = ySize;
	if (zSize >= ySize && zSize >= xSize) _size = zSize;

	_size = 9;

	int chunksToAllocate = _size * _size * _size;

	int i;
	for (i = 0; i < chunksToAllocate; ++i)
	{
		Chunk* chunk = new Chunk(chunkSize, blockSize);
		initChunk(chunk);
		_chunks.push_back(chunk);
	}
}

ChunkManager::~ChunkManager()
{
	ChunkList::iterator iter;
	for (iter = _chunks.begin(); iter != _chunks.end(); ++iter)
	{
		delete *iter;
	}
}
