
#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "TextureManager.h"
#include "FontManager.h"

#include <string>

namespace engine
{
	class ResourceManager
	{
	public:

		ResourceManager();
		~ResourceManager();

		TextureManager& getTextureManager();
		FontManager&    getFontManager();

		void setResourcePath(const std::string& path);
		std::string getResourcePath();

	private:
		TextureManager _textureManager;
		FontManager    _fontManager;

		std::string _resourcePath;
	};
}

#endif
