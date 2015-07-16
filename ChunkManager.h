
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
		void update();

		void updateVisiblityList(sgl::Frustum& frustum);

		void translate(float x, float y, float z);
		void rotate(float x, float y, float z);
		void scale(float s);

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
		void setLightSource(int x, int y, int z, int r, int g, int b);

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

		sgl::Matrix4& getWorldTransform();

		void setAtlasName(const std::string& name);
		std::string getAtlasName();

		void setRenderDebug(bool d);

		bool boundingVolumeOutOfDate();

	private:

		ChunkList _chunks;
		ChunkSet  _chunkRenderSet;
		ChunkSet  _chunkVisibleSet;
		ChunkSet  _chunkRebuildSet;

		int _blockX;           // number of block in the x direction
		int _blockY;           // number of block in the y direction
		int _blockZ;           // number of block in the z direction

		int   _blocksPerChunk; // number of blocks in one dimesnsion of the chunk
		float _blockSize;      // half size of block

		int _size;

		int _rebuildsPerFrame;

		bool _renderDebug;

		sgl::Matrix4 _worldTransform;

		std::string _atlasName;

		bool _updateBoundingVolume;

	private:
		Chunk& getChunk(int x, int y, int z);

		void rebuildChunks();

		void updateChunkVolumes();

		void allocateChunks(int chunkSize, float blockSize);
		void setChunkNeighbors(Chunk& chunk);

		void updateCallback(Chunk* chunk);

	};
}

#endif
