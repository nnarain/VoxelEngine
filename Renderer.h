
#ifndef RENDERER_H
#define RENDERER_H

#include "ChunkManager.h"
#include "TextureManager.h"

#include <SGL/GL/ShaderProgram.h>
#include <SGL/GL/Texture.h>

#include <SGL/Math/Matrix4.h>

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
		void render(ChunkManager& chunkManager, sgl::Matrix4& VP);
		void end();

		TextureManager& getTextureManager();

	private:
		sgl::ShaderProgram _chunkShader;

		TextureManager _textureManager;

	};
}

#endif