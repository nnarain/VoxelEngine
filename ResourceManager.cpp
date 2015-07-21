
#include "ResourceManager.h"

using namespace engine;

ResourceManager::ResourceManager()
{
}

TextureManager& ResourceManager::getTextureManager()
{
	return _textureManager;
}

ResourceManager::~ResourceManager()
{
}
