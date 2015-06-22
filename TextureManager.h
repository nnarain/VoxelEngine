
#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include "TextureAtlas.h"

#include <SGL/GL/Texture.h>

#include <map>
#include <string>

namespace engine
{
	class TextureManager
	{
	public:

		TextureManager();
		~TextureManager();

		void addAtlas(const char *atlasPath);
		void addTexture(const char *texturePath);

		sgl::Texture& getTexture(const std::string& name);
		TextureAtlas& getAtlas(const std::string& name);

	private:

		typedef std::map<std::string, sgl::Texture*> TextureMap;
		typedef std::map<std::string, TextureAtlas*> TextureAtlasMap;

		TextureMap _textureMap;
		TextureAtlasMap _textureAtlasMap;

	private:
		std::string getNameFromPath(const char *path);
		void dispose();

	};
}

#endif
