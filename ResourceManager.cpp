
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

void ResourceManager::setResourcePath(const std::string& path)
{
	_resourcePath = path;
}

std::string ResourceManager::getResourcePath()
{
	return _resourcePath;
}

ResourceManager::~ResourceManager()
{
}
