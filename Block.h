
#ifndef BLOCK_H
#define BLOCK_H

#include <SGL/Math/Vector2.h>
#include <SGL/Math/Vector3.h>
#include <SGL/Graphics/Color.h>

#include <cstdint>

#define GET_LIGHT_LEVEL_R(light) ((light & ~0xFFF0))
#define GET_LIGHT_LEVEL_G(light) ((light & ~0xFF0F) >> 4 )
#define GET_LIGHT_LEVEL_B(light) ((light & ~0xF0FF) >> 8 )
#define GET_LIGHT_LEVEL_A(light) ((light & ~0x0FFF) >> 12)

#define SET_LIGHT_LEVEL_R(light, level) ((light) |= ((level) & ~0xFFF0))
#define SET_LIGHT_LEVEL_G(light, level) ((light) |= ((level) & ~0xFF0F) << 4 )
#define SET_LIGHT_LEVEL_B(light, level) ((light) |= ((level) & ~0xF0FF) << 8 )
#define SET_LIGHT_LEVEL_A(light, level) ((light) |= ((level) & ~0x0FFF) << 12)


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

	uint8_t  t;
	uint8_t  x;
	uint8_t  y;
	uint8_t  z;
	uint32_t light;
};

#endif
