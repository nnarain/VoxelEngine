
#ifndef CHUNK_H
#define CHUNK_H

#include "Block.h"

#include <SGL/GL/Mesh.h>

#include <vector>
#include <string>

namespace engine
{
	class Chunk
	{
	public:

		Chunk(int size);
		Chunk(int size, float blockSize);
		~Chunk();

		Block& getBlock(int x, int y, int z);

		void build();

		void render();

		void setBlock(int x, int y, int z, int t);

		bool isSetup(void) const;

		bool shouldRender(void);

		bool hasLocation(void) const;

		void setLocation(int x, int y, int z);
		sgl::Vector3 getLocation(void);

		void setAtlasName(const std::string& name);

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

		std::string _atlasName;

		int _size;
		float _blockSize;

		bool _dirty;
		bool _shouldRender;

		bool _hasLocation;

		void createCubeMesh(Block& block, bool l, bool r, bool t, bool b, bool n, bool far);

		Triangle makeFace(Vertex& v1, Vertex& v2, Vertex& v3, Block block, bool firstHalf);
	};
}

#endif
