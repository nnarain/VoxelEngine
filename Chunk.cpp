
#include "Chunk.h"

#include "VoxelEngine.h"

#include <SGL/Math/Vector4.h>

#include <iostream>
#include <queue>

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

void Chunk::setLightSource(int x, int y, int z, int r, int g, int b, int a)
{
	setLightLevel(x, y, z, r, g, b, a);

	_lightSourceList.emplace_back((uint8_t)x, (uint8_t)y, (uint8_t)z, getBlock(x, y, z)->light, this);
}

void Chunk::setLightLevel(int x, int y, int z, int r, int g, int b, int a)
{
	Block* block = getBlock(x, y, z);

	SET_LIGHT_LEVEL_R(block->light, r);
	SET_LIGHT_LEVEL_G(block->light, g);
	SET_LIGHT_LEVEL_B(block->light, b);
	SET_LIGHT_LEVEL_A(block->light, a);

	// notify the parent that this chunk needs to be rebuilt
	_updateCallback(this);
	_dirty = true;
}

uint32_t Chunk::getLightLevel(int x, int y, int z)
{
	return getBlock(x, y, z)->light;
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

	ColorRGB32f color = getBlockColor(block);

	Vector3 ux(1, 0, 0);
	Vector3 uy(0, 1, 0);
	Vector3 uz(0, 0, 1);

	Vertex vLBN(lbn, calculatePerVertexNormal(-ux, -uy, -uz, l, b, n), color);
	Vertex vRBN(rbn, calculatePerVertexNormal( ux, -uy, -uz, r, b, n), color);
	Vertex vLTN(ltn, calculatePerVertexNormal(-ux,  uy, -uz, l, t, n), color);
	Vertex vRTN(rtn, calculatePerVertexNormal( ux,  uy, -uz, r, t, n), color);

	Vertex vLBF(lbf, calculatePerVertexNormal(-ux, -uy, uz, l, b, f),  color);
	Vertex vRBF(rbf, calculatePerVertexNormal( ux, -uy, uz, r, b, f),  color);
	Vertex vLTF(ltf, calculatePerVertexNormal(-ux,  uy, uz, l, t, f),  color);
	Vertex vRTF(rtf, calculatePerVertexNormal( ux,  uy, uz, r, t, f),  color);

	// near face
	if (n)
	{
		_buffer.push_back(makeFace(vLBN, vRBN, vRTN, block, true));
		_buffer.push_back(makeFace(vRTN, vLTN, vLBN, block, false));
	}

	// far face
	if (f)
	{
		_buffer.push_back(makeFace(vLBF, vRBF, vRTF, block, true));
		_buffer.push_back(makeFace(vRTF, vLTF, vLBF, block, false));
	}

	// left face
	if (l)
	{
		_buffer.push_back(makeFace(vLBN, vLTN, vLTF, block, true));
		_buffer.push_back(makeFace(vLTF, vLBF, vLBN, block, false));
	}

	// right face
	if (r)
	{
		_buffer.push_back(makeFace(vRBN, vRTN, vRTF, block, true));
		_buffer.push_back(makeFace(vRTF, vRBF, vRBN, block, false));
	}

	// top face
	if (t)
	{
		_buffer.push_back(makeFace(vLTN, vLTF, vRTF, block, true));
		_buffer.push_back(makeFace(vRTF, vRTN, vLTN, block, false));
	}

	// bottom face
	if (b)
	{
		_buffer.push_back(makeFace(vLBN, vLBF, vRBF, block, true));
		_buffer.push_back(makeFace(vRBF, vRBN, vLBN, block, false));
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

Chunk::Face Chunk::makeFace(Vertex& v1, Vertex& v2, Vertex& v3, Block block, bool firstHalf)
{
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
	std::vector<LightNode>::iterator iter;
	for (iter = _lightSourceList.begin(); iter != _lightSourceList.end(); ++iter)
	{
		// breathe first searh queue of light nodes
		std::queue<LightNode> bfsLightQueue;

		bfsLightQueue.push(*iter);

		while (!bfsLightQueue.empty())
		{
			// get the next node
			LightNode node = bfsLightQueue.front();

			// extract information

			int x = (int)node.x;
			int y = (int)node.y;
			int z = (int)node.z;
			
			uint32_t lightLevel = node.lightLevel;

			Chunk* owner = node.owner;

			// remove node from queue
			bfsLightQueue.pop();

			//
			LightNode adjacentNode;

			adjacentNode = owner->getLightNode(x - 1, y, z);

			if (adjacentNode.owner != nullptr && spreadLight(adjacentNode, lightLevel))
				bfsLightQueue.push(adjacentNode);

			adjacentNode = owner->getLightNode(x + 1, y, z);

			if (adjacentNode.owner != nullptr && spreadLight(adjacentNode, lightLevel))
				bfsLightQueue.push(adjacentNode);

			adjacentNode = owner->getLightNode(x, y + 1, z);

			if (adjacentNode.owner != nullptr && spreadLight(adjacentNode, lightLevel))
				bfsLightQueue.push(adjacentNode);

			adjacentNode = owner->getLightNode(x, y - 1, z);

			if (adjacentNode.owner != nullptr && spreadLight(adjacentNode, lightLevel))
				bfsLightQueue.push(adjacentNode);

			adjacentNode = owner->getLightNode(x, y, z - 1);

			if (adjacentNode.owner != nullptr && spreadLight(adjacentNode, lightLevel))
				bfsLightQueue.push(adjacentNode);

			adjacentNode = owner->getLightNode(x, y, z + 1);

			if (adjacentNode.owner != nullptr && spreadLight(adjacentNode, lightLevel))
				bfsLightQueue.push(adjacentNode);


		}
	}
}

bool Chunk::spreadLight(LightNode& node, uint32_t level)
{
	int r1 = GET_LIGHT_LEVEL_R(level);
	int g1 = GET_LIGHT_LEVEL_G(level);
	int b1 = GET_LIGHT_LEVEL_B(level);

	int r2 = GET_LIGHT_LEVEL_R(node.lightLevel);
	int g2 = GET_LIGHT_LEVEL_G(node.lightLevel);
	int b2 = GET_LIGHT_LEVEL_B(node.lightLevel);

	int newR = r2, newG = g2, newB = b2;

	bool propagate = false;

	if (r2 + 2 <= r1 && r1 > 0)
	{
		SET_LIGHT_LEVEL_R(node.lightLevel, r1 - 1);
		newR = r1 - 1;
		propagate = true;
	}

	if (g2 + 2 <= g1 && g1 > 0)
	{
		SET_LIGHT_LEVEL_G(node.lightLevel, g1 - 1);
		newG = g1 - 1;
		propagate = true;
	}

	if (b2 + 2 <= b1 && b1 > 0)
	{
		SET_LIGHT_LEVEL_B(node.lightLevel, b1 - 1);
		newB = b1 - 1;
		propagate = true;
	}

	node.owner->setLightLevel(node.x, node.y, node.z, newR, newG, newB, 0);

	return propagate;
}

Chunk::LightNode Chunk::getLightNode(int x, int y, int z)
{
	if (x < 0)
	{
		if (this->left != nullptr)
			return this->left->getLightNode(_size + x, y, z);
		else
			return LightNode(0, 0, 0, 0, nullptr);
	}
	else if (x >= _size)
	{
		if (this->right != nullptr)
			return this->right->getLightNode(x - _size, y, z);
		else
			return LightNode(0, 0, 0, 0, nullptr);
	}

	if (y < 0)
	{
		if (this->bottom != nullptr)
			return this->bottom->getLightNode(x, _size + y, z);
		else
			return LightNode(0, 0, 0, 0, nullptr);
	}
	else if (y >= _size)
	{
		if (this->top != nullptr)
			return this->top->getLightNode(x, y - _size, z);
		else
			return LightNode(0, 0, 0, 0, nullptr);
	}

	if (z < 0)
	{
		if (this->near != nullptr)
			return this->near->getLightNode(x, y, _size + z);
		else
			return LightNode(0, 0, 0, 0, nullptr);
	}
	else if (z >= _size)
	{
		if (this->far != nullptr)
			return this->far->getLightNode(x, y, z - _size);
		else
			return LightNode(0, 0, 0, 0, nullptr);
	}

	Block* block = getBlock(x, y, z);

	return LightNode(block->x, block->y, block->z, block->light, this);
}

ColorRGB32f Chunk::getBlockColor(Block& block)
{
	uint8_t ri = GET_LIGHT_LEVEL_R(block.light);
	uint8_t gi = GET_LIGHT_LEVEL_G(block.light);
	uint8_t bi = GET_LIGHT_LEVEL_B(block.light);

	float rf = (float)ri / 15.0f;
	float gf = (float)gi / 15.0f;
	float bf = (float)bi / 15.0f;

	return ColorRGB32f(rf, gf, bf);
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

Chunk::~Chunk()
{

}
