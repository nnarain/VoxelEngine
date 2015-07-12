
#include "Chunk.h"

#include "VoxelEngine.h"

#include <SGL/Math/Vector4.h>

#include <iostream>

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

	_mesh.create(sizeof(Vertex));
}

void Chunk::setBlock(int x, int y, int z, int t)
{
	Block& block = getBlock(x, y, z);
	block.t = t;

	_dirty = true;
}

Block& Chunk::getBlock(int x, int y, int z)
{
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (z < 0) z = 0;

	int idx = (x * _size) + (y * _size * _size) + z;

	Block& block = _blocks[idx];

	if (!_hasLocationFlags[idx])
	{
		block.x = x;
		block.y = y;
		block.z = z;
	}

	return block;
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

	// iterate over each block and created the mesh

	int x, y, z;

	for (x = 0; x < _size; ++x)
	{
		for (y = 0; y < _size; ++y)
		{
			for (z = 0; z < _size; ++z)
			{
				Block& block = getBlock(x, y, z);

				// add this block if it is active
				if (block.t)
				{
					// check if the adjacent blocks are active, if so don't create the joining face in the mesh
					bool l = true, r = true, t = true, b = true, n = true, f = true;

					if ((int)block.x - 1 >= 0 && block.x + 1 < _size)
						l = getBlock(block.x - 1, block.y, block.z).t == 0;

					if ((int)block.x - 1 >= 0 && block.x + 1 < _size)
						r = getBlock(block.x + 1, block.y, block.z).t == 0;

					if ((int)block.y - 1 >= 0 && block.y + 1 < _size)
						t = getBlock(block.x, block.y + 1, block.z).t == 0;

					if ((int)block.y - 1 >= 0 && block.y + 1 < _size)
						b = getBlock(block.x, block.y - 1, block.z).t == 0;

					if ((int)block.z - 1 >= 0 && block.z + 1 < _size)
						n = getBlock(block.x, block.y, block.z -1).t == 0;

					if ((int)block.z - 1 >= 0 && block.z + 1< _size)
						f = getBlock(block.x, block.y, block.z + 1).t == 0;

					createCubeMesh(block, l, r, t, b, n, f);
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

	// near face
	if (n)
	{
		_buffer.push_back(makeFace(Vertex(lbn), Vertex(rbn), Vertex(rtn), block, true));
		_buffer.push_back(makeFace(Vertex(rtn), Vertex(ltn), Vertex(lbn), block, false));
	}

	// far face
	if (f)
	{
		_buffer.push_back(makeFace(Vertex(lbf), Vertex(rbf), Vertex(rtf), block, true));
		_buffer.push_back(makeFace(Vertex(rtf), Vertex(ltf), Vertex(lbf), block, false));
	}

	// left face
	if (l)
	{
		_buffer.push_back(makeFace(Vertex(lbn), Vertex(ltn), Vertex(ltf), block, true));
		_buffer.push_back(makeFace(Vertex(ltf), Vertex(lbf), Vertex(lbn), block, false));
	}

	// right face
	if (r)
	{
		_buffer.push_back(makeFace(Vertex(rbn), Vertex(rtn), Vertex(rtf), block, true));
		_buffer.push_back(makeFace(Vertex(rtf), Vertex(rbf), Vertex(rbn), block, false));
	}

	// top face
	if (t)
	{
		_buffer.push_back(makeFace(Vertex(ltn), Vertex(ltf), Vertex(rtf), block, true));
		_buffer.push_back(makeFace(Vertex(rtf), Vertex(rtn), Vertex(ltn), block, false));
	}

	// bottom face
	if (b)
	{
		_buffer.push_back(makeFace(Vertex(lbn), Vertex(lbf), Vertex(rbf), block, true));
		_buffer.push_back(makeFace(Vertex(rbf), Vertex(rbn), Vertex(lbn), block, false));
	}
}

Chunk::Face Chunk::makeFace(Vertex& v1, Vertex& v2, Vertex& v3, Block block, bool firstHalf)
{
	// calculate the normal of the triangle

	// calculate 2 edge vectors of the triangle
	Vector3 u = v2.position - v1.position;
	Vector3 v = v3.position - v1.position;

	// the perpendicular vector to the edges is the normal

	// normal
	Vector3 n;
	n.set(u).cross(v).normalize();

	v1.normal.set(n);
	v2.normal.set(n);
	v3.normal.set(n);

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

	//
	return Face(v1, v2, v3);
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

Chunk::~Chunk()
{

}
