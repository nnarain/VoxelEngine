
#ifndef CHUNKMANAGER_H
#define CHUNKMANGER_H

#include "Chunk.h"
#include "FPSCamera.h"

#include <vector>
#include <set>

namespace engine
{
	typedef std::vector<Chunk*> ChunkList;
	typedef std::set<Chunk*>    ChunkSet;

	class ChunkManager
	{
	public:

		/**
			(x, y, z) - grid dimensions in blocks

			chunkSize - number of blocks per chunk
			blockSize - half the render size of the block
		*/
		ChunkManager(int x, int y, int z, int chunkSize, float blockSize);

		/**
			(x, y, z) - grid dimensions in blocks
		*/
		ChunkManager(int x, int y, int z);

		~ChunkManager();

		/**
			Update which chunks should be rendered using a camera frustum
		*/
		void update(FPSCamera& camera);

		void updateVisiblityList(sgl::Frustum& frustum);

		/**
			render visible chunks
		*/
		void render();

		/**
			Get the block at the specified grid position
		*/
		Block getBlock(int x, int y, int z);

		/**
			Set the block type at position (x, y, z)
		*/
		void setBlock(int x, int y, int z, int t);

		/**
		
		*/
		Block getBlockFromWorldPosition(const sgl::Vector3& position);

		Chunk& getChunkFromWorldPosition(const sgl::Vector3& position);
		Chunk& getChunkFromWorldPosition(float x, float y, float z);
		
		/**
		*/
		Block getBlockFromWorldPosition(float x, float y, float z);

		int getBlockX() const;
		int getBlockY() const;
		int getBlockZ() const;

	private:

		int _blockX;      // number of block in the x direction
		int _blockY;      // number of block in the y direction
		int _blockZ;      // number of block in the z direction

		int   _chunkSize;   // number of blocks in one dimesnsion of the chunk
		float _blockSize; // half size of block

		int _rebuildsPerFrame;

		ChunkList _chunks;
		ChunkSet  _chunkRenderSet;
		ChunkSet  _chunkVisibleSet;
		ChunkSet  _chunkRebuildSet;

		Chunk& getChunk(int x, int y, int z);

		void rebuildChunks();

		void fillFrustumVolume(float volume, ChunkSet& adjacent);

		void allocateChunks(int chunkSize, float blockSize);

	};
}

#endif
