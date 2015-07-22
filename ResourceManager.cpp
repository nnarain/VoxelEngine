
#include "ResourceManager.h"

using namespace engine;

ResourceManager::ResourceManager()
{
}

TextureManager& ResourceManager::getTextureManager()
{
	return _textureManager;
}

FontManager& ResourceManager::getFontManager()
{
	return _fontManager;
}

ResourceManager::~ResourceManager()
{
}
