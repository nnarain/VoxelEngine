
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

		// Structure of a block's face
		struct Face
		{
			Face(Vertex& v1, Vertex& v2, Vertex& v3) : v1(v1), v2(v2), v3(v3)
			{
			}

			Vertex v1;
			Vertex v2;
			Vertex v3;
		};

		// structure used for lighting
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

		/**
			Initialize chunk with the number of block per each axis
		*/
		Chunk(int size);

		/**
			Initialize chunk with the number of block per each axis and block render size
		*/
		Chunk(int size, float blockSize);
		~Chunk();

		/**
			@return the block at (x, y, z) in this chunk
		*/
		Block* getBlock(int x, int y, int z);

		/**
			Get block at location (x, y, z).

			The difference between this function and Block::getBlock is that it accounts for the 
			coordinates to overflow into neighboring chunks
		*/
		Block* getAdjacentBlock(int x, int y, int z);

		/**
			Build the mesh for this chunk
		*/
		void build();

		/**
			Render this chunk
		*/
		void render();

		/**
			Set the block at (x, y, z) to type t
		*/
		void setBlock(int x, int y, int z, int t);

		/**
			Set the light color value at (x, y, z)
		*/
		void setLightSource(int x, int y, int z, int r, int g, int b);

		/**
			Remove light at (x, y, z)
		*/
		void removeLight(int x, int y, int z);

		/**
			check if the chunk is set up
		*/
		bool isSetup(void) const;

		/**
			@return is the chunk should be rendered
		*/
		bool shouldRender(void);

		/**
			check if this chunk's location has already been set
		*/
		bool hasLocation(void) const;

		/**
			Set the location of this chunk
		*/
		void setLocation(int x, int y, int z);

		/**
			@return the location of this chunk in the grid
		*/
		sgl::Vector3 getLocation(void);

		/**
			Set the texture atlas name of this chunk
		*/
		void setAtlasName(const std::string& name);

		/**
			Calculate the bounding volume of this chunk
		*/
		void calculateBounds(sgl::Matrix4& worldTransform);

		/**
			@return the bounding volume of this chunk
		*/
		sgl::Sphere& getBounds();

		/**
			Set the callback for when this chunk needs to be updated
		*/
		void setUpdateCallback(std::function<void(Chunk*)> callback);

		/**
			Mark this chunk for an update
		*/
		void markForUpdate();

		/**
			@return the list of light sources
		*/
		LightMap& getLightSourceMap();

	public:

		Chunk* left;   // the chunk left neighbor
		Chunk* right;  // the chunk right neighbor
		Chunk* top;    // the chunk top neighbor
		Chunk* bottom; // the chunk bottom neighbor
		Chunk* near;   // the chunk near neighbor
		Chunk* far;    // the chunk far neighbor

	private:

		// mesh for this chunk
		sgl::Mesh _mesh;

		// buffer of faces for each block of the mesh
		std::vector<Face> _buffer;

		// the chunk offest
		sgl::Vector3 _offset;

		// list of blocks
		std::vector<Block> _blocks;
		// flag indicating if a blocks location has already been set
		std::vector<bool> _hasLocationFlags;

		// list of light sources
		LightMap _lightSourceList;
		// list os light sources to be removed
		LightMap _lightRemovalList;

		// the texture atlas name for this chunk
		std::string _atlasName;

		// callback for when the chunk needs to be updated
		std::function<void(Chunk*)> _updateCallback;

		// the number of blocks per axis of this chunk
		int _size;
		// the block render size
		float _blockSize;

		// flag indicated that the chunk need rebuilding
		bool _dirty;
		// flag for is the chunk shoud be rendered
		bool _shouldRender;

		// flag for if the chunks offest has already been set
		bool _hasLocation;

		// spherical bounding area of the chunk
		sgl::Sphere _bounds;

	private:

		// create the mesh for this block
		void createCubeMesh(Block& block, bool l, bool r, bool t, bool b, bool n, bool far);

		/**
			make a face of a block using the 3 vertices
		*/
		Face makeFace(Vertex& v1, Vertex& v2, Vertex& v3, Block block, bool firstHalf, sgl::ColorRGB32f& color);
		sgl::Vector3 calculatePerVertexNormal(sgl::Vector3 x, sgl::Vector3 y, sgl::Vector3 z, bool adjacentX, bool adjacentY, bool adjacentZ);
		Face textureFace(Vertex& v1, Vertex& v2, Vertex& v3, Block block, bool firstHalf);

		/**
			Propagate the light sources
		*/
		void propagateLight();
		bool propagateLightPerFace(LightNode& source, LightNode& block, BlockFace face);
		bool spreadLight(LightNode& node, BlockFace face, light_t level);
		void setLightLevel(Block* block, int r, int g, int b, BlockFace face);

		LightNode getLightNode(int x, int y, int z);
		LightNode getLightNode(Block* block, BlockFace face);

		void removeLightSources();
		void clearBlockLight(Block* block);
		void clearLightNode(LightNode& node, std::queue<LightNode>& queue, std::map<Block*, int>& intensities, int intensity);

		sgl::ColorRGB32f getBlockColor(Block& block, BlockFace face);
	};
}

#endif
