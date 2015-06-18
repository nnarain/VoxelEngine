
#ifndef CHUNK_H
#define CHUNK_H

#include "Block.h"

#include <SGL/GL/Mesh.h>

#include <vector>

namespace engine
{
	class Chunk
	{
	public:

		Chunk(int size);
		Chunk(int size, float blockSize);
		~Chunk();

		Block getBlock(int x, int y, int z);

		void build();

		void render();

		void setBlock(int x, int y, int z, int t);

		bool isSetup(void) const;

		bool hasLocation(void) const;

		void setLocation(int x, int y, int z);
		sgl::Vector3 getLocation(void);

	private:
		//
		struct Triangle
		{
			Triangle(Vertex& v1, Vertex& v2, Vertex& v3) : v1(v1), v2(v2), v3(v3)
			{
			}

			Vertex v1;
			Vertex v2;
			Vertex v3;
		};

		sgl::Mesh _mesh;

		std::vector<Triangle> _buffer;

		sgl::Vector3 _offset;

		std::vector<Block> _blocks;
		std::vector<bool> _hasLocationFlags;

		int _size;
		float _blockSize;

		bool _dirty;
		bool _shouldRender;

		bool _hasLocation;

		void createCubeMesh(Block& block);

		bool isSurrounded(Block& block);

		Triangle makeFace(Vertex& v1, Vertex& v2, Vertex& v3, uint8_t t);
	};
}

#endif
