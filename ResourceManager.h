
#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "TextureManager.h"

namespace engine
{
	class ResourceManager
	{
	public:

		ResourceManager();
		~ResourceManager();

		TextureManager& getTextureManager();

	private:
		TextureManager _textureManager;
	};
}

#endif
