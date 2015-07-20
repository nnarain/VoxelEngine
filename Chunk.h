
#ifndef CHUNK_H
#define CHUNK_H

#include "Block.h"

#include <SGL/GL/Mesh.h>
#include <SGL/Math/Sphere.h>
#include <SGL/Math/Matrix4.h>

#include <vector>
#include <queue>
#include <set>
#include <map>
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

		void setLightSource(int x, int y, int z, int r, int g, int b);

		void removeLight(int x, int y, int z);

		bool isSetup(void) const;

		bool shouldRender(void);

		bool hasLocation(void) const;

		void setLocation(int x, int y, int z);
		sgl::Vector3 getLocation(void);

		void setAtlasName(const std::string& name);

		void calculateBounds(sgl::Matrix4& worldTransform);

		sgl::Sphere& getBounds();

		void setUpdateCallback(std::function<void(Chunk*)> callback);
		void markForUpdate();

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
			LightNode(Block* block, Chunk* owner) : block(block), owner(owner)
			{
			}

			LightNode() : LightNode(nullptr, nullptr)
			{
			}

			Block* block;
			Chunk* owner;
		};

		typedef std::map<Block*, LightNode> LightMap;

		sgl::Mesh _mesh;

		std::vector<Face> _buffer;

		sgl::Vector3 _offset;

		std::vector<Block> _blocks;
		std::vector<bool> _hasLocationFlags;

		LightMap _lightSourceList;
		LightMap _lightRemovalList;

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

		Face makeFace(Vertex& v1, Vertex& v2, Vertex& v3, Block block, bool firstHalf, sgl::ColorRGB32f& color);
		sgl::Vector3 calculatePerVertexNormal(sgl::Vector3 x, sgl::Vector3 y, sgl::Vector3 z, bool adjacentX, bool adjacentY, bool adjacentZ);
		Face textureFace(Vertex& v1, Vertex& v2, Vertex& v3, Block block, bool firstHalf);

		void propagateLight();
		bool propagateLightPerFace(LightNode& source, LightNode& block, BlockFace face);
		bool spreadLight(LightNode& node, BlockFace face, light_t level);
		void setLightLevel(Block* block, int r, int g, int b, BlockFace face);

		LightNode getLightNode(int x, int y, int z);
		LightNode getLightNode(Block* block, BlockFace face);

		void removeLightSources();
		void clearBlockLight(Block* block);

		sgl::ColorRGB32f getBlockColor(Block& block, BlockFace face);
	};
}

#endif
