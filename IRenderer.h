
#ifndef IRENDERER_H
#define IRENDERER_H

#include "ChunkManager.h"
#include "TextureManager.h"

#include <SGL/Math/Matrix4.h>

namespace engine
{
	class IRenderer
	{
	public:

		IRenderer();

		virtual void init()                                               = 0;

		virtual void begin()                                              = 0;
		virtual void render(ChunkManager& chunkManager, sgl::Matrix4& VP) = 0;
		virtual void end()                                                = 0;

		TextureManager& getTextureManager();

	protected:
		TextureManager _textureManager;
	};
}

#endif