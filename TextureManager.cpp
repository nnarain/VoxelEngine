
#include "TextureManager.h"

#include <SGL/Util/Image.h>
#include <SGL/Util/Exception.h>

#include <boost/filesystem.hpp>

#include <iostream>
#include <exception>

using namespace engine;
using namespace sgl;

TextureManager::TextureManager()
{
}

void TextureManager::addAtlas(const char *atlasPath)
{
	using namespace boost::filesystem;

	// load the texture
	addTexture(atlasPath);

	// get the file name without extension
	std::string baseName = getNameFromPath(atlasPath);
	std::string packFilename = baseName + ".json";

	// get the pack file 
	path packFilePath = path(atlasPath).parent_path();
	packFilePath.append(packFilename);

	if (exists(packFilePath))
	{
		Texture* texture = _textureMap[baseName];

		_textureAtlasMap[baseName] = new TextureAtlas(texture, packFilePath.string());
	}
	else
	{
		throw std::exception(std::string("Error. pack file does not exist: " + packFilename).c_str());
	}
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

TextureAtlas& TextureManager::getAtlas(const std::string& name)
{
	return *(_textureAtlasMap[name]);
}

void TextureManager::dispose()
{
	TextureMap::iterator iter;
	for (iter = _textureMap.begin(); iter != _textureMap.end(); ++iter)
	{
		delete iter->second;
	}

	TextureAtlasMap::iterator atlasIter;
	for (atlasIter = _textureAtlasMap.begin(); atlasIter != _textureAtlasMap.end(); ++atlasIter)
	{
		delete atlasIter->second;
	}
}

TextureManager::~TextureManager()
{
	dispose();
}