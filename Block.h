
#ifndef BLOCK_H
#define BLOCK_H

#include <SGL/Math/Vector2.h>
#include <SGL/Math/Vector3.h>
#include <SGL/Graphics/Color.h>

#include <cstdint>

#define R_MASK 0x000F    // mask for light R channel
#define G_MASK 0x00F0    // mask for light G channel
#define B_MASK 0x0F00    // mask for light B channel

#define CHNL_MASK 0x000F // mask for a single channel of light
#define CHNL_BITS 4      // bits per channel

// get the R channel value
#define GET_LIGHT_LEVEL_R(light) ((light & R_MASK))
// get the G channel value
#define GET_LIGHT_LEVEL_G(light) ((light & G_MASK) >> CHNL_BITS )
// get the B channel value
#define GET_LIGHT_LEVEL_B(light) ((light & B_MASK) >> (CHNL_BITS * 2) )

// set the R channel value
#define SET_LIGHT_LEVEL_R(light, level) (light) = ( ( (light) & ~R_MASK ) | ((level) & CHNL_MASK) )
// get the G channel value
#define SET_LIGHT_LEVEL_G(light, level) (light) = ( ( (light) & ~G_MASK ) | ((level) & CHNL_MASK) << (CHNL_BITS) )
// get the B channel value
#define SET_LIGHT_LEVEL_B(light, level) (light) = ( ( (light) & ~B_MASK ) | ((level) & CHNL_MASK) << (CHNL_BITS * 2) )

// light data type
typedef uint16_t light_t;

/**
	A single vertex for mesh creation

	Contains position, normal, texCoord and color attributes
*/
struct Vertex
{
	Vertex(sgl::Vector3& pos, sgl::Vector3& n, sgl::ColorRGB32f c) : position(pos), normal(n), color(c)
	{
	}

	Vertex(sgl::Vector3& pos, sgl::Vector3& n) : Vertex(pos, n, sgl::ColorRGB32f(1, 1, 1))
	{
	}

	Vertex(sgl::Vector3& pos) : Vertex(pos, sgl::Vector3(0, 0, 0), sgl::ColorRGB32f(1,1,1))
	{
	}

	sgl::Vector3     position;
	sgl::Vector3     normal;
	sgl::Vector2     texCoord;
	sgl::ColorRGB32f color;
};

struct Block
{
	Block() : Block(0, 0, 0, 0)
	{
	}

	Block(uint8_t t, uint8_t x, uint8_t y, uint8_t z) : t(t), x(x), y(y), z(z)
	{
		lights[0] = 0;
		lights[1] = 0;
		lights[2] = 0;
		lights[3] = 0;
		lights[4] = 0;
		lights[5] = 0;
	}

	uint8_t  t;        // the block type
	uint8_t  x;        // block x value in chunk
	uint8_t  y;        // block y value in chunk
	uint8_t  z;        // block z value in chunk
	light_t lights[6]; // light values for each face
};

// block face number
enum class BlockFace
{
	LEFT, RIGHT,
	TOP,  BOTTOM,
	NEAR, FAR
};

// check if a block is opaque
static bool isBlockOpaque(Block& block)
{
	return block.t != 0;
}

// check if the block has a light value
static bool hasLight(Block& block)
{
	return (block.lights[0] | block.lights[1] | block.lights[2] | block.lights[3] | block.lights[4] | block.lights[5]) != 0;
}

#endif
