
#ifndef RENDERER_H
#define RENDERER_H

#include "ChunkManager.h"

#include <SGL/GL/ShaderProgram.h>
#include <SGL/GL/Texture.h>

namespace engine
{
	/**
	*/
	class Renderer
	{
	public:

		Renderer(void);
		~Renderer();

		void init();

		void begin();
		void render(ChunkManager& chunkManager);
		void end();

	private:
		sgl::ShaderProgram _chunkShader;
	};
}

#endif