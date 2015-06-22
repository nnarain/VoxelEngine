
#ifndef CHUNKMANAGER_H
#define CHUNKMANAGER_H

#include "Chunk.h"
#include "FPSCamera.h"

#include <SGL/Math/Matrix4.h>

#include <vector>
#include <set>
#include <string>

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
		ChunkManager(int x, int y, int z, int blocksPerChunk, float blockSize, const char *atlasName);

		/**
			(x, y, z) - grid dimensions in blocks
		*/
		ChunkManager(int x, int y, int z, const char *atlasName);

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

		sgl::Matrix4& getModelMatrix();

		void setAtlasName(const std::string& name);
		std::string getAtlasName();

	private:

		int _blockX;      // number of block in the x direction
		int _blockY;      // number of block in the y direction
		int _blockZ;      // number of block in the z direction

		int   _blocksPerChunk;   // number of blocks in one dimesnsion of the chunk
		float _blockSize; // half size of block


		int _size;

		int _rebuildsPerFrame;

		sgl::Matrix4 _modelMatrix;

		std::string _atlasName;

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
