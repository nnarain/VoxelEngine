
#ifndef TEXTUREATLAS_H
#define TEXTUREATLAS_H

#include "Block.h"

#include <SGL/GL/Texture.h>

#include <vector>

namespace engine
{
	class TextureAtlas
	{
	public:

		TextureAtlas(sgl::Texture* texture, const std::string& packFilename);
		~TextureAtlas();

		void load(sgl::Texture* texture, const std::string& packFilename);

		sgl::Texture::TextureRegion& getRegion(Block block);

	private:
		std::vector<sgl::Texture::TextureRegion> _regions;
	
	};
}

#endif
