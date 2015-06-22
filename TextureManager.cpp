
#include "TextureManager.h"

#include <SGL/Util/Image.h>
#include <SGL/Util/Exception.h>

#include <boost/filesystem.hpp>

#include <iostream>

using namespace engine;
using namespace sgl;

TextureManager::TextureManager()
{
}

void TextureManager::addTexture(const char *texturePath)
{
	Texture* texture = new Texture(Texture::Target::TEXTURE2D);

	try
	{
		texture->bind();

		Image::load(*texture, texturePath);

		texture->parameter(Texture::ParamName::MAG_FILTER, Texture::Param::LINEAR);
		texture->parameter(Texture::ParamName::MIN_FILTER, Texture::Param::LINEAR);

		texture->unbind();
	}
	catch (sgl::Exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	_textureMap[getNameFromPath(texturePath)] = texture;
}

std::string TextureManager::getNameFromPath(const char *path)
{
	return boost::filesystem::basename(path);
}

Texture& TextureManager::getTexture(const std::string& name)
{
	return *(_textureMap[name]);
}

void TextureManager::dispose()
{
	TextureMap::iterator iter;
	for (iter = _textureMap.begin(); iter != _textureMap.end(); ++iter)
	{
		delete iter->second;
	}
}

TextureManager::~TextureManager()
{
	dispose();
}