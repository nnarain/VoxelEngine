
#include "Chunk.h"

#include "VoxelEngine.h"

#include <SGL/Math/Vector4.h>

#include <iostream>
#include <memory>

using namespace engine;
using namespace sgl;

Chunk::Chunk(int size) : Chunk(size, 1)
{
}

Chunk::Chunk(int size, float blockSize) : 
	_mesh(Mesh::Type::TRIANGLES, Buffer::Usage::STATIC_DRAW),
	_size(size),
	_blockSize(blockSize),
	_dirty(true),
	_shouldRender(false),
	_hasLocation(false),

	left(nullptr),
	right(nullptr),
	top(nullptr),
	bottom(nullptr),
	near(nullptr),
	far(nullptr)
{
	// allocate blocks and flags
	_blocks.resize(size * size * size);
	_hasLocationFlags.resize(size * size * size, false);

	// setup mesh
	_mesh.addAttribute(VertexAttribute(0, 3));
	_mesh.addAttribute(VertexAttribute(1, 3));
	_mesh.addAttribute(VertexAttribute(2, 2));
	_mesh.addAttribute(VertexAttribute(3, 3));

	_mesh.create(sizeof(Vertex));
}

void Chunk::setBlock(int x, int y, int z, int t)
{
	Block* block = getBlock(x, y, z);
	block->t = t;

	_dirty = true;
}

Block* Chunk::getBlock(int x, int y, int z)
{
	int idx = (x * _size) + (y * _size * _size) + z;

	Block* block = &_blocks[idx];

	if (!_hasLocationFlags[idx])
	{
		block->x = x;
		block->y = y;
		block->z = z;
	}

	return block;
}

Block* Chunk::getAdjacentBlock(int x, int y, int z)
{
	if (x < 0)
	{
		if (this->left != nullptr)
			return this->left->getAdjacentBlock(_size + x, y, z);
		else
			return nullptr;
	}
	else if (x >= _size)
	{
		if (this->right != nullptr)
			return this->right->getAdjacentBlock(x - _size, y, z);
		else
			return nullptr;
	}

	if (y < 0)
	{
		if (this->bottom != nullptr)
			return this->bottom->getAdjacentBlock(x, _size + y, z);
		else
			return nullptr;
	}
	else if (y >= _size)
	{
		if (this->top != nullptr)
			return this->top->getAdjacentBlock(x, y - _size, z);
		else
			return nullptr;
	}

	if (z < 0)
	{
		if (this->near != nullptr)
			return this->near->getAdjacentBlock(x, y, _size + z);
		else
			return nullptr;

	}
	else if (z >= _size)
	{
		if (this->far != nullptr)
			return this->far->getAdjacentBlock(x, y, z - _size);
		else
			return nullptr;
	}

	return getBlock(x, y, z);
}

void Chunk::setLightSource(int x, int y, int z, int r, int g, int b)
{
	Block* block = getBlock(x, y, z);

	setLightLevel(block, r, g, b, BlockFace::LEFT);
	setLightLevel(block, r, g, b, BlockFace::RIGHT);
	setLightLevel(block, r, g, b, BlockFace::TOP);
	setLightLevel(block, r, g, b, BlockFace::BOTTOM);
	setLightLevel(block, r, g, b, BlockFace::NEAR);
	setLightLevel(block, r, g, b, BlockFace::FAR);


	_lightSourceList[block] = LightNode(block, this);
}

void Chunk::removeLight(int x, int y, int z)
{
	Block* block = getBlock(x, y, z);

	_lightRemovalList[block] = LightNode(block, this);

	_updateCallback(this);
	_dirty = true;
}

void Chunk::setLightLevel(Block* block, int r, int g, int b, BlockFace face)
{
	int idx = static_cast<int>(face);

	SET_LIGHT_LEVEL_R(block->lights[idx], r);
	SET_LIGHT_LEVEL_G(block->lights[idx], g);
	SET_LIGHT_LEVEL_B(block->lights[idx], b);
}

void Chunk::render()
{
	_mesh.bind();
	_mesh.draw();
	_mesh.unbind();
}

void Chunk::build()
{
	_shouldRender = false;

	// clear existing data from the buffer
	_buffer.clear();

	// remove any sources that are listed
	removeLightSources();

	// propagate any light sources in the chunk
	propagateLight();

	// iterate over each block and created the mesh

	int x, y, z;

	for (x = 0; x < _size; ++x)
	{
		for (y = 0; y < _size; ++y)
		{
			for (z = 0; z < _size; ++z)
			{
				Block* block = getBlock(x, y, z);

				// add this block if it is active
				if (block->t)
				{
					// check if the adjacent blocks are active, if so don't create the joining face in the mesh
					bool l, r, t, b, n, f;

					Block* adjacentBlock;

					adjacentBlock = getAdjacentBlock(block->x - 1, block->y, block->z);
					l = (adjacentBlock == nullptr || adjacentBlock->t == 0);

					adjacentBlock = getAdjacentBlock(block->x + 1, block->y, block->z);
					r = (adjacentBlock == nullptr || adjacentBlock->t == 0);

					adjacentBlock = getAdjacentBlock(block->x, block->y + 1, block->z);
					t = (adjacentBlock == nullptr || adjacentBlock->t == 0);

					adjacentBlock = getAdjacentBlock(block->x, block->y - 1, block->z);
					b = (adjacentBlock == nullptr || adjacentBlock->t == 0);

					adjacentBlock = getAdjacentBlock(block->x, block->y, block->z - 1);
					n = (adjacentBlock == nullptr || adjacentBlock->t == 0);

					adjacentBlock = getAdjacentBlock(block->x, block->y, block->z + 1);
					f = (adjacentBlock == nullptr || adjacentBlock->t == 0);

					createCubeMesh(*block, l, r, t, b, n, f);
					_shouldRender = true;
				}
			}
		}
	}

	_mesh.setDrawCount(_buffer.size() * 3);

	Buffer& vbo = _mesh.getVBO();
	vbo.bind();

	if (_buffer.size() > 0)
	{
		vbo.setData(&_buffer[0], _buffer.size() * sizeof(Chunk::Face));
	}
	else
	{
		vbo.setData(0, 0);
	}

	vbo.unbind();

	_dirty = false;
}

void Chunk::createCubeMesh(Block& block, bool l, bool r, bool t, bool b, bool n, bool f)
{
	// create the 8 vertices that make up the cube
	// l - left, r - right  (x axis)
	// t - top , b - bottom (y axis)
	// n - near, f - far    (z axis)

	float x = (float)block.x;
	float y = (float)block.y;
	float z = (float)block.z;

	float X = _offset.x * (_size * _blockSize * 2);
	float Y = _offset.y * (_size * _blockSize * 2);
	float Z = _offset.z * (_size * _blockSize * 2);

	Vector3 lbn((x * 2 * _blockSize + X) - _blockSize, (y * 2 * _blockSize + Y) - _blockSize, (z * 2 * _blockSize + Z) - _blockSize);
	Vector3 rbn((x * 2 * _blockSize + X) + _blockSize, (y * 2 * _blockSize + Y) - _blockSize, (z * 2 * _blockSize + Z) - _blockSize);
	Vector3 ltn((x * 2 * _blockSize + X) - _blockSize, (y * 2 * _blockSize + Y) + _blockSize, (z * 2 * _blockSize + Z) - _blockSize);
	Vector3 rtn((x * 2 * _blockSize + X) + _blockSize, (y * 2 * _blockSize + Y) + _blockSize, (z * 2 * _blockSize + Z) - _blockSize);
	Vector3 lbf((x * 2 * _blockSize + X) - _blockSize, (y * 2 * _blockSize + Y) - _blockSize, (z * 2 * _blockSize + Z) + _blockSize);
	Vector3 rbf((x * 2 * _blockSize + X) + _blockSize, (y * 2 * _blockSize + Y) - _blockSize, (z * 2 * _blockSize + Z) + _blockSize);
	Vector3 ltf((x * 2 * _blockSize + X) - _blockSize, (y * 2 * _blockSize + Y) + _blockSize, (z * 2 * _blockSize + Z) + _blockSize);
	Vector3 rtf((x * 2 * _blockSize + X) + _blockSize, (y * 2 * _blockSize + Y) + _blockSize, (z * 2 * _blockSize + Z) + _blockSize);

	Vector3 ux(1, 0, 0);
	Vector3 uy(0, 1, 0);
	Vector3 uz(0, 0, 1);

	Vertex vLBN(lbn, calculatePerVertexNormal(-ux, -uy, -uz, l, b, n));
	Vertex vRBN(rbn, calculatePerVertexNormal( ux, -uy, -uz, r, b, n));
	Vertex vLTN(ltn, calculatePerVertexNormal(-ux,  uy, -uz, l, t, n));
	Vertex vRTN(rtn, calculatePerVertexNormal( ux,  uy, -uz, r, t, n));

	Vertex vLBF(lbf, calculatePerVertexNormal(-ux, -uy, uz, l, b, f));
	Vertex vRBF(rbf, calculatePerVertexNormal( ux, -uy, uz, r, b, f));
	Vertex vLTF(ltf, calculatePerVertexNormal(-ux,  uy, uz, l, t, f));
	Vertex vRTF(rtf, calculatePerVertexNormal( ux,  uy, uz, r, t, f));

	// near face
	if (n)
	{
		_buffer.push_back(makeFace(vLBN, vRBN, vRTN, block, true, getBlockColor(block, BlockFace::NEAR)));
		_buffer.push_back(makeFace(vRTN, vLTN, vLBN, block, false, getBlockColor(block, BlockFace::NEAR)));
	}

	// far face
	if (f)
	{
		_buffer.push_back(makeFace(vLBF, vRBF, vRTF, block, true, getBlockColor(block, BlockFace::FAR)));
		_buffer.push_back(makeFace(vRTF, vLTF, vLBF, block, false, getBlockColor(block, BlockFace::FAR)));
	}

	// left face
	if (l)
	{
		_buffer.push_back(makeFace(vLBN, vLTN, vLTF, block, true, getBlockColor(block, BlockFace::LEFT)));
		_buffer.push_back(makeFace(vLTF, vLBF, vLBN, block, false, getBlockColor(block, BlockFace::LEFT)));
	}

	// right face
	if (r)
	{
		_buffer.push_back(makeFace(vRBN, vRTN, vRTF, block, true, getBlockColor(block, BlockFace::RIGHT)));
		_buffer.push_back(makeFace(vRTF, vRBF, vRBN, block, false, getBlockColor(block, BlockFace::RIGHT)));
	}

	// top face
	if (t)
	{
		_buffer.push_back(makeFace(vLTN, vLTF, vRTF, block, true, getBlockColor(block, BlockFace::TOP)));
		_buffer.push_back(makeFace(vRTF, vRTN, vLTN, block, false, getBlockColor(block, BlockFace::TOP)));
	}

	// bottom face
	if (b)
	{
		_buffer.push_back(makeFace(vLBN, vLBF, vRBF, block, true, getBlockColor(block, BlockFace::BOTTOM)));
		_buffer.push_back(makeFace(vRBF, vRBN, vLBN, block, false, getBlockColor(block, BlockFace::BOTTOM)));
	}
}

Vector3 Chunk::calculatePerVertexNormal(Vector3 x, Vector3 y, Vector3 z, bool adjacentX, bool adjacentY, bool adjacentZ)
{
	Vector3 result;
	if (adjacentX) result += x;
	if (adjacentY) result += y;
	if (adjacentZ) result += z;

	return result.normalize();
}

Chunk::Face Chunk::makeFace(Vertex& v1, Vertex& v2, Vertex& v3, Block block, bool firstHalf, ColorRGB32f& color)
{
	v1.color = color;
	v2.color = color;
	v3.color = color;

	return textureFace(v1, v2, v3, block, firstHalf);
}

Chunk::Face Chunk::textureFace(Vertex& v1, Vertex& v2, Vertex& v3, Block block, bool firstHalf)
{
	// set vertex texture coordinates
	Texture::TextureRegion region = VoxelEngine::getEngine()->getRenderer().getTextureManager().getAtlas(_atlasName).getRegion(block);

	if (firstHalf)
	{
		v1.texCoord = region.topLeft;
		v2.texCoord = region.bottomLeft;
		v3.texCoord = region.bottomRight;
	}
	else
	{
		v1.texCoord = region.bottomRight;
		v2.texCoord = region.topRight;
		v3.texCoord = region.topLeft;
	}

	return Face(v1, v2, v3);
}

void Chunk::propagateLight()
{
	// keep a set of all chunks that are affected by the light propagation
	ChunkSet updateSet;

	for (auto& source : _lightSourceList)
	{
		// breathe first searh queue of light nodes
		std::queue<LightNode> bfsLightQueue;

		bfsLightQueue.push(source.second);

		while (!bfsLightQueue.empty())
		{

			// get the next node
			LightNode node = bfsLightQueue.front();

			// extract information

			int x = (int)node.block->x;
			int y = (int)node.block->y;
			int z = (int)node.block->z;

			Chunk* owner = node.owner;
			updateSet.insert(owner);

			// remove node from queue
			bfsLightQueue.pop();

			//
			LightNode adjacentNode;

			adjacentNode = owner->getLightNode(x - 1, y, z);

			if (adjacentNode.owner != nullptr && propagateLightPerFace(node, adjacentNode, BlockFace::LEFT))
				bfsLightQueue.push(adjacentNode);

			adjacentNode = owner->getLightNode(x + 1, y, z);

			if (adjacentNode.owner != nullptr && propagateLightPerFace(node, adjacentNode, BlockFace::RIGHT))
				bfsLightQueue.push(adjacentNode);

			adjacentNode = owner->getLightNode(x, y + 1, z);

			if (adjacentNode.owner != nullptr && propagateLightPerFace(node, adjacentNode, BlockFace::TOP))
				bfsLightQueue.push(adjacentNode);

			adjacentNode = owner->getLightNode(x, y - 1, z);

			if (adjacentNode.owner != nullptr && propagateLightPerFace(node, adjacentNode, BlockFace::BOTTOM))
				bfsLightQueue.push(adjacentNode);

			adjacentNode = owner->getLightNode(x, y, z - 1);

			if (adjacentNode.owner != nullptr && propagateLightPerFace(node, adjacentNode, BlockFace::NEAR))
				bfsLightQueue.push(adjacentNode);

			adjacentNode = owner->getLightNode(x, y, z + 1);

			if (adjacentNode.owner != nullptr && propagateLightPerFace(node, adjacentNode, BlockFace::FAR))
				bfsLightQueue.push(adjacentNode);
		}
	}

	// update all chunks affected
	for (auto& chunk : updateSet)
		chunk->markForUpdate();


}

bool Chunk::propagateLightPerFace(LightNode& source, LightNode& adjacent, BlockFace face)
{
	Block* sourceBlock   = source.block;
	Block* adjacentBlock = adjacent.block;

	bool propagate = false;

	// check if the light should propagate through the neighbour block
	if (isBlockOpaque(*adjacentBlock))
	{
		bool f1, f2, f3, f4, f5;

		if (face == BlockFace::LEFT || face == BlockFace::RIGHT)
		{
			f1 = spreadLight(adjacent, BlockFace::NEAR,   sourceBlock->lights[(int)BlockFace::NEAR]);
			f2 = spreadLight(adjacent, BlockFace::FAR,    sourceBlock->lights[(int)BlockFace::FAR]);
			f3 = spreadLight(adjacent, BlockFace::TOP,    sourceBlock->lights[(int)BlockFace::TOP]);
			f4 = spreadLight(adjacent, BlockFace::BOTTOM, sourceBlock->lights[(int)BlockFace::BOTTOM]);

			if (face == BlockFace::LEFT)
			{
				f5 = spreadLight(adjacent, BlockFace::RIGHT, sourceBlock->lights[(int)face]);
			}
			else
			{
				f5 = spreadLight(adjacent, BlockFace::LEFT, sourceBlock->lights[(int)face]);
			}
		}
		else if (face == BlockFace::TOP || face == BlockFace::BOTTOM)
		{
			f1 = spreadLight(adjacent, BlockFace::LEFT,  sourceBlock->lights[(int)BlockFace::LEFT]);
			f2 = spreadLight(adjacent, BlockFace::RIGHT, sourceBlock->lights[(int)BlockFace::RIGHT]);
			f3 = spreadLight(adjacent, BlockFace::NEAR,  sourceBlock->lights[(int)BlockFace::NEAR]);
			f4 = spreadLight(adjacent, BlockFace::FAR,   sourceBlock->lights[(int)BlockFace::FAR]);

			if (face == BlockFace::TOP)
			{
				f5 = spreadLight(adjacent, BlockFace::BOTTOM, sourceBlock->lights[(int)face]);
			}
			else
			{
				f5 = spreadLight(adjacent, BlockFace::TOP, sourceBlock->lights[(int)face]);
			}
		}
		else //if (face == BlockFace::NEAR || face == BlockFace::FAR)
		{
			f1 = spreadLight(adjacent, BlockFace::LEFT,   sourceBlock->lights[(int)BlockFace::LEFT]);
			f2 = spreadLight(adjacent, BlockFace::RIGHT,  sourceBlock->lights[(int)BlockFace::RIGHT]);
			f3 = spreadLight(adjacent, BlockFace::TOP,    sourceBlock->lights[(int)BlockFace::TOP]);
			f4 = spreadLight(adjacent, BlockFace::BOTTOM, sourceBlock->lights[(int)BlockFace::BOTTOM]);

			if (face == BlockFace::NEAR)
			{
				f5 = spreadLight(adjacent, BlockFace::FAR, sourceBlock->lights[(int)face]);
			}
			else
			{
				f5 = spreadLight(adjacent, BlockFace::NEAR, sourceBlock->lights[(int)face]);
			}
		}

		propagate = f1 || f2 || f3 || f4 || f5;
	}
	else
	{
		light_t sourceLevel = sourceBlock->lights[static_cast<int>(face)];

		bool f1, f2, f3, f4, f5, f6;

		f1 = spreadLight(adjacent, BlockFace::LEFT,   sourceLevel);
		f2 = spreadLight(adjacent, BlockFace::RIGHT,  sourceLevel);
		f3 = spreadLight(adjacent, BlockFace::TOP,    sourceLevel);
		f4 = spreadLight(adjacent, BlockFace::BOTTOM, sourceLevel);
		f5 = spreadLight(adjacent, BlockFace::NEAR,   sourceLevel);
		f6 = spreadLight(adjacent, BlockFace::FAR,    sourceLevel);

		propagate = f1 || f2 || f3 || f4 || f5 || f6;
	}

	return propagate;
}

bool Chunk::spreadLight(LightNode& node, BlockFace face, light_t level)
{
	static Vector3 neighbours[] = {
		Vector3(-1,  0,  0), // left
		Vector3( 1,  0,  0), // right
		Vector3( 0,  1,  0), // top
		Vector3( 0, -1,  0), // bottom
		Vector3( 0,  0, -1), // near
		Vector3( 0,  0,  1)  // far
	};

	int idx = static_cast<int>(face);

	Vector3 neighbour = neighbours[idx];
	Block* neighbourBlock = node.owner->getAdjacentBlock(
		(int)node.block->x + (int)neighbour.x,
		(int)node.block->y + (int)neighbour.y,
		(int)node.block->z + (int)neighbour.z
	);

	bool isNeighbourActive = (neighbourBlock != nullptr) && (neighbourBlock->t != 0);

	bool propagate = false;

	if ((node.block->t == 0) || !isNeighbourActive)
	{
		int r1 = GET_LIGHT_LEVEL_R(level);
		int g1 = GET_LIGHT_LEVEL_G(level);
		int b1 = GET_LIGHT_LEVEL_B(level);

		int r2 = GET_LIGHT_LEVEL_R(node.block->lights[idx]);
		int g2 = GET_LIGHT_LEVEL_G(node.block->lights[idx]);
		int b2 = GET_LIGHT_LEVEL_B(node.block->lights[idx]);

		int newR = r2, newG = g2, newB = b2;

		if (r2 + 2 <= r1)
		{
			newR = r1 - 1;
			propagate = true;
		}

		if (g2 + 2 <= g1)
		{
			newG = g1 - 1;
			propagate = true;
		}

		if (b2 + 2 <= b1)
		{
			newB = b1 - 1;
			propagate = true;
		}

		node.owner->setLightLevel(node.block, newR, newG, newB, face);
	}

	return propagate;
}

Chunk::LightNode Chunk::getLightNode(int x, int y, int z)
{
	if (x < 0)
	{
		if (this->left != nullptr)
			return this->left->getLightNode(_size + x, y, z);
		else
			return LightNode(nullptr, nullptr);
	}
	else if (x >= _size)
	{
		if (this->right != nullptr)
			return this->right->getLightNode(x - _size, y, z);
		else
			return LightNode(nullptr, nullptr);
	}

	if (y < 0)
	{
		if (this->bottom != nullptr)
			return this->bottom->getLightNode(x, _size + y, z);
		else
			return LightNode(nullptr, nullptr);
	}
	else if (y >= _size)
	{
		if (this->top != nullptr)
			return this->top->getLightNode(x, y - _size, z);
		else
			return LightNode(nullptr, nullptr);
	}

	if (z < 0)
	{
		if (this->near != nullptr)
			return this->near->getLightNode(x, y, _size + z);
		else
			return LightNode(nullptr, nullptr);
	}
	else if (z >= _size)
	{
		if (this->far != nullptr)
			return this->far->getLightNode(x, y, z - _size);
		else
			return LightNode(nullptr, nullptr);
	}

	Block* block = getBlock(x, y, z);

	return LightNode(block, this);
}

void Chunk::removeLightSources()
{
	ChunkSet updateSet;

	for (auto& iter : _lightRemovalList)
	{
		std::queue<LightNode> lightQueue;
		std::map<Block*, int> intensities;

		// add the source node to the FIFO queue
		lightQueue.push(iter.second);

		// get the highest light intensity of each channel of each face

		int sourceIntensity = 0;

		int i;
		for (i = 0; i < 6; ++i)
		{
			int r = GET_LIGHT_LEVEL_R(iter.second.block->lights[i]);
			int g = GET_LIGHT_LEVEL_G(iter.second.block->lights[i]);
			int b = GET_LIGHT_LEVEL_B(iter.second.block->lights[i]);

			sourceIntensity = std::max(std::max(std::max(r, g), b), sourceIntensity);
		}

		// clear the source block light
		clearBlockLight(iter.second.block);

		// set the intensity value in the map
		intensities[iter.second.block] = sourceIntensity;

		while (!lightQueue.empty())
		{
			LightNode source = lightQueue.front();

			int x = (int)source.block->x;
			int y = (int)source.block->y;
			int z = (int)source.block->z;

			lightQueue.pop();

			int intensity = intensities[source.block];

			if (intensity > 0)
			{
				LightNode adjacent;

				adjacent = source.owner->getLightNode(x - 1, y, z);
				if (adjacent.block != nullptr && intensities.find(adjacent.block) == intensities.end())
				{
					lightQueue.push(adjacent);
					intensities[adjacent.block] = intensity - 1;
					clearBlockLight(adjacent.block);
					updateSet.insert(adjacent.owner);
				}

				adjacent = source.owner->getLightNode(x + 1, y, z);
				if (adjacent.block != nullptr && intensities.find(adjacent.block) == intensities.end())
				{
					lightQueue.push(adjacent);
					intensities[adjacent.block] = intensity - 1;
					clearBlockLight(adjacent.block);
					updateSet.insert(adjacent.owner);
				}

				adjacent = source.owner->getLightNode(x, y + 1, z);
				if (adjacent.block != nullptr && intensities.find(adjacent.block) == intensities.end())
				{
					lightQueue.push(adjacent);
					intensities[adjacent.block] = intensity - 1;
					clearBlockLight(adjacent.block);
					updateSet.insert(adjacent.owner);
				}

				adjacent = source.owner->getLightNode(x, y - 1, z);
				if (adjacent.block != nullptr && intensities.find(adjacent.block) == intensities.end())
				{
					lightQueue.push(adjacent);
					intensities[adjacent.block] = intensity - 1;
					clearBlockLight(adjacent.block);
					updateSet.insert(adjacent.owner);
				}

				adjacent = source.owner->getLightNode(x, y, z - 1);
				if (adjacent.block != nullptr && intensities.find(adjacent.block) == intensities.end())
				{
					lightQueue.push(adjacent);
					intensities[adjacent.block] = intensity - 1;
					clearBlockLight(adjacent.block);
					updateSet.insert(adjacent.owner);
				}

				adjacent = source.owner->getLightNode(x, y, z + 1);
				if (adjacent.block != nullptr && intensities.find(adjacent.block) == intensities.end())
				{
					lightQueue.push(adjacent);
					intensities[adjacent.block] = intensity - 1;
					clearBlockLight(adjacent.block);
					updateSet.insert(adjacent.owner);
				}
			}
		}

		_lightSourceList.erase(iter.second.block);
	}

	for (auto& chunk : updateSet)
	{
		chunk->markForUpdate();
	}
}

void Chunk::clearBlockLight(Block* block)
{
	block->lights[0] = 0;
	block->lights[1] = 0;
	block->lights[2] = 0;
	block->lights[3] = 0;
	block->lights[4] = 0;
	block->lights[5] = 0;
}

ColorRGB32f Chunk::getBlockColor(Block& block, BlockFace face)
{
	light_t light = block.lights[static_cast<int>(face)];

	// get the channel values
	uint8_t ri = GET_LIGHT_LEVEL_R(light);
	uint8_t gi = GET_LIGHT_LEVEL_G(light);
	uint8_t bi = GET_LIGHT_LEVEL_B(light);

	// convert to floating point
	float rf = (float)ri / (float)CHNL_MASK;
	float gf = (float)gi / (float)CHNL_MASK;
	float bf = (float)bi / (float)CHNL_MASK;

	//
	return ColorRGB32f(rf, gf, bf);
}

Chunk::LightNode Chunk::getLightNode(Block* block, BlockFace face)
{
	switch (face)
	{
	case BlockFace::LEFT:
		return getLightNode((int)block->x - 1, (int)block->y, (int)block->z);
	case BlockFace::RIGHT:
		return getLightNode((int)block->x + 1, (int)block->y, (int)block->z);
	case BlockFace::TOP:
		return getLightNode((int)block->x, (int)block->y + 1, (int)block->z);
	case BlockFace::BOTTOM:
		return getLightNode((int)block->x, (int)block->y - 1, (int)block->z);
	case BlockFace::NEAR:
		return getLightNode((int)block->x, (int)block->y, (int)block->z - 1);
	case BlockFace::FAR:
		return getLightNode((int)block->x, (int)block->y, (int)block->z + 1);

	default:
		return LightNode(nullptr, nullptr);
	}
}

bool Chunk::isSetup(void) const
{
	return _dirty == false;
}

bool Chunk::shouldRender(void)
{
	return _shouldRender;
}

bool Chunk::hasLocation(void) const
{
	return _hasLocation;
}

void Chunk::setLocation(int x, int y, int z)
{
	_offset.x = (float)x;
	_offset.y = (float)y;
	_offset.z = (float)z;

	_hasLocation = true;
}

Vector3 Chunk::getLocation(void)
{
	return _offset;
}

void Chunk::setAtlasName(const std::string& name)
{
	_atlasName = name;
}

void Chunk::calculateBounds(Matrix4& worldTransform)
{
	// calculate this chunks AABB

	// the main offset for the chunk
	float X = _offset.x * (_size * _blockSize * 2);
	float Y = _offset.y * (_size * _blockSize * 2);
	float Z = _offset.z * (_size * _blockSize * 2);

	float maxBlockOffset = (float)(_size - 1);

	// lbn vertex for block 0, 0, 0
	Vector4 lbn;
	lbn.x = (0 * 2 * _blockSize + X) - _blockSize;
	lbn.y = (0 * 2 * _blockSize + Y) - _blockSize;
	lbn.z = (0 * 2 * _blockSize + Z) - _blockSize;
	lbn.w = 1;

	// rtf vertex for block 15, 15, 15
	Vector4 rtf;
	rtf.x = (maxBlockOffset * 2 * _blockSize + X) + _blockSize;
	rtf.y = (maxBlockOffset * 2 * _blockSize + Y) + _blockSize;
	rtf.z = (maxBlockOffset * 2 * _blockSize + Z) + _blockSize;
	rtf.w = 1;

	Vector4 min = worldTransform * lbn;
	Vector4 max = worldTransform * rtf;

	Vector4 center = (max + min) / 2.0f;
	_bounds.center = Vector3(center.x, center.y, center.z);
	_bounds.radius = (Vector3(max.x, max.y, max.z) - _bounds.center).length();
}

Sphere& Chunk::getBounds()
{
	return _bounds;
}

void Chunk::setUpdateCallback(std::function<void(Chunk*)> callback)
{
	_updateCallback = callback;
}

void Chunk::markForUpdate()
{
	_updateCallback(this);
	_dirty = true;
}

Chunk::~Chunk()
{

}
