
#include "FontManager.h"

#include <boost/filesystem.hpp>

using namespace sgl;

namespace engine
{
	FontManager::FontManager()
	{
	}

	void FontManager::addFont(const char* filename, unsigned int cols, unsigned int rows, bool flip)
	{
		BitmapFont* font = new BitmapFont();
		font->init(filename, cols, rows, flip);

		std::string name = boost::filesystem::basename(filename);

		_fontMap[name] = font;
	}

	FontManager::~FontManager()
	{
		// delete the font objects
		for (auto& iter : _fontMap)
			delete iter.second;
	}
}
