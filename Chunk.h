
#ifndef CHUNK_H
#define CHUNK_H

#include "Block.h"

#include <SGL/GL/Mesh.h>
#include <SGL/Math/Sphere.h>
#include <SGL/Math/Matrix4.h>

#include <vector>
#include <set>
#include <string>
#include <functional>

namespace engine
{
	class Chunk
	{
	public:

		Chunk(int size);
		Chunk(int size, float blockSize);
		~Chunk();

		Block* getBlock(int x, int y, int z);
		Block* getAdjacentBlock(int x, int y, int z);

		void build();

		void render();

		void setBlock(int x, int y, int z, int t);

		void setLightSource(int x, int y, int z, int r, int g, int b, int a);
		void setLightLevel(int x, int y, int z, int r, int g, int b, int a);
		
		uint32_t getLightLevel(int x, int y, int z);

		bool isSetup(void) const;

		bool shouldRender(void);

		bool hasLocation(void) const;

		void setLocation(int x, int y, int z);
		sgl::Vector3 getLocation(void);

		void setAtlasName(const std::string& name);

		void calculateBounds(sgl::Matrix4& worldTransform);

		sgl::Sphere& getBounds();

		void setUpdateCallback(std::function<void(Chunk*)> callback);

	public:

		Chunk* left;
		Chunk* right;
		Chunk* top;
		Chunk* bottom;
		Chunk* near;
		Chunk* far;

	private:
		//
		struct Face
		{
			Face(Vertex& v1, Vertex& v2, Vertex& v3) : v1(v1), v2(v2), v3(v3)
			{
			}

			Vertex v1;
			Vertex v2;
			Vertex v3;
		};

		struct LightNode
		{
			LightNode(uint8_t x, uint8_t y, uint8_t z, uint32_t level, Chunk* chunk) : x(x), y(y), z(z), lightLevel(level), owner(chunk)
			{
			}

			LightNode() : LightNode(0, 0, 0, 0, nullptr)
			{
			}

			uint8_t x;
			uint8_t y;
			uint8_t z;
			uint32_t lightLevel;
			Chunk* owner;
		};

		sgl::Mesh _mesh;

		std::vector<Face> _buffer;

		sgl::Vector3 _offset;

		std::vector<Block> _blocks;
		std::vector<bool> _hasLocationFlags;

		std::vector<LightNode> _lightSourceList;

		std::string _atlasName;

		std::function<void(Chunk*)> _updateCallback;

		int _size;
		float _blockSize;

		bool _dirty;
		bool _shouldRender;

		bool _hasLocation;

		sgl::Sphere _bounds;

	private:

		void createCubeMesh(Block& block, bool l, bool r, bool t, bool b, bool n, bool far);

		Face makeFace(Vertex& v1, Vertex& v2, Vertex& v3, Block block, bool firstHalf);
		sgl::Vector3 calculatePerVertexNormal(sgl::Vector3 x, sgl::Vector3 y, sgl::Vector3 z, bool adjacentX, bool adjacentY, bool adjacentZ);
		Face textureFace(Vertex& v1, Vertex& v2, Vertex& v3, Block block, bool firstHalf);

		void propagateLight();
		bool spreadLight(LightNode& node, uint32_t level);

		LightNode getLightNode(int x, int y, int z);

		sgl::ColorRGB32f getBlockColor(Block& block);
	};
}

#endif
