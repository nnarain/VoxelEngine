
#include "TextureAtlas.h"

#include <fstream>
#include <sstream>
#include <cassert>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

using namespace engine;
using namespace sgl;

TextureAtlas::TextureAtlas(Texture* texture, const std::string& packFilename)
{
	load(texture, packFilename);
}

void TextureAtlas::load(Texture* texture, const std::string& packFilename)
{
	using namespace boost::property_tree;

	std::ifstream file(packFilename);

	if (!file.good()) throw std::exception(std::string("Could not open file: " + packFilename).c_str());

	//
	std::stringstream buffer;
	buffer << file.rdbuf();

	//
	ptree pt;

	// parse out the json file
	read_json(buffer, pt);

	//
	BOOST_FOREACH(ptree::value_type& v, pt.get_child("atlas"))
	{
		float x = v.second.get<float>("x");
		float y = v.second.get<float>("y");
		float w = v.second.get<float>("w");
		float h = v.second.get<float>("h");

		Texture::TextureRegion region = texture->region(x, y, w, h);

		_regions.push_back(region);
	}
}

Texture::TextureRegion& TextureAtlas::getRegion(Block block)
{
	assert(block.t > 0);
	return _regions[block.t - 1];
}

TextureAtlas::~TextureAtlas()
{
}
