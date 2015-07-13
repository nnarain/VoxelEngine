
#ifndef BLOCK_H
#define BLOCK_H

#include <SGL/Math/Vector2.h>
#include <SGL/Math/Vector3.h>

#include <cstdint>

struct Vertex
{
	Vertex(sgl::Vector3& pos, sgl::Vector3& n) : position(pos), normal(n)
	{
	}

	Vertex(sgl::Vector3& pos) : Vertex(pos, sgl::Vector3(0, 0, 0))
	{
	}

	sgl::Vector3 position;
	sgl::Vector3 normal;
	sgl::Vector2 texCoord;
};

struct Block
{
	Block() : Block(0, 0, 0, 0)
	{
	}

	Block(uint8_t t, uint8_t x, uint8_t y, uint8_t z) : t(t), x(x), y(y), z(z)
	{
	}

	uint8_t t;
	uint8_t x;
	uint8_t y;
	uint8_t z;
};

#endif
