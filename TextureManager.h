
#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

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

		void addTexture(const char *texturePath);

		sgl::Texture& getTexture(const std::string& name);

	private:

		typedef std::map<std::string, sgl::Texture*> TextureMap;

		TextureMap _textureMap;

	private:
		std::string getNameFromPath(const char *path);
		void dispose();

	};
}

#endif
