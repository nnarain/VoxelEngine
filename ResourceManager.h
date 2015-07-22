
#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "TextureManager.h"
#include "FontManager.h"

namespace engine
{
	class ResourceManager
	{
	public:

		ResourceManager();
		~ResourceManager();

		TextureManager& getTextureManager();
		FontManager&    getFontManager();

	private:
		TextureManager _textureManager;
		FontManager    _fontManager;
	};
}

#endif
