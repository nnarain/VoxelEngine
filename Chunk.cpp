
#include "Chunk.h"

#include "VoxelEngine.h"

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
	_hasLocation(false)
{
	_blocks.resize(size * size * size);
	_hasLocationFlags.resize(size * size * size, false);

	//
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

				// add this block if it is active and is not surrounded
				if (block.t && !isSurrounded(block))
				{
					createCubeMesh(block);
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
		vbo.setData(&_buffer[0], _buffer.size() * sizeof(Chunk::Triangle));
	}
	else
	{
		vbo.setData(0, 0);
	}

	vbo.unbind();

	_dirty = false;
}

void Chunk::createCubeMesh(Block& block)
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
	_buffer.push_back(makeFace(Vertex(lbn), Vertex(rbn), Vertex(rtn), block, true));
	_buffer.push_back(makeFace(Vertex(rtn), Vertex(ltn), Vertex(lbn), block, false));

	// far face
	_buffer.push_back(makeFace(Vertex(lbf), Vertex(rbf), Vertex(rtf), block, true));
	_buffer.push_back(makeFace(Vertex(rtf), Vertex(ltf), Vertex(lbf), block, false));

	// left face
	_buffer.push_back(makeFace(Vertex(lbn), Vertex(ltn), Vertex(ltf), block, true));
	_buffer.push_back(makeFace(Vertex(ltf), Vertex(lbf), Vertex(lbn), block, false));

	// right face
	_buffer.push_back(makeFace(Vertex(rbn), Vertex(rtn), Vertex(rtf), block, true));
	_buffer.push_back(makeFace(Vertex(rtf), Vertex(rbf), Vertex(rbn), block, false));

	// top face
	_buffer.push_back(makeFace(Vertex(ltn), Vertex(ltf), Vertex(rtf), block, true));
	_buffer.push_back(makeFace(Vertex(rtf), Vertex(rtn), Vertex(ltn), block, false));

	// bottom face
	_buffer.push_back(makeFace(Vertex(lbn), Vertex(lbf), Vertex(rbf), block, true));
	_buffer.push_back(makeFace(Vertex(rbf), Vertex(rbn), Vertex(lbn), block, false));
}

Chunk::Triangle Chunk::makeFace(Vertex& v1, Vertex& v2, Vertex& v3, Block block, bool firstHalf)
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
		v1.texCoord = region.bottomLeft;
		v2.texCoord = region.topLeft;
		v3.texCoord = region.topRight;
	}
	else
	{
		v1.texCoord = region.bottomLeft;
		v2.texCoord = region.bottomRight;
		v3.texCoord = region.topRight;
	}

	//
	return Triangle(v1, v2, v3);
}

bool Chunk::isSurrounded(Block& block)
{
	int x = block.x;
	int y = block.y;
	int z = block.z;

	if ((x - 1 > 0 && x + 1 < _size) && (x - 1 > 0 && x + 1 < _size) && (y - 1 > 0 && y + 1 < _size) && (z - 1 > 0 && z + 1 < _size))
	{
		// get the adjacent blocks

		Block& l = getBlock(x - 1, y, z);
		Block& r = getBlock(x + 1, y, z);
		Block& t = getBlock(x, y + 1, z);
		Block& b = getBlock(x, y - 1, z);
		Block& n = getBlock(x, y, z - 1);
		Block& f = getBlock(x, y, z + 1);

		return l.t && r.t && t.t && b.t && n.t && f.t;
	}

	return false;
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

Chunk::~Chunk()
{

}
