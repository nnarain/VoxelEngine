
#ifndef FONTMANAGER_H
#define FONTMANAGER_H

#include <SGL/Graphics/BitmapFont.h>

#include <map>
#include <string>

namespace engine
{
	class FontManager
	{
	public:

		FontManager();
		~FontManager();

		void addFont(const char* filename, unsigned int cols, unsigned int rows, bool flip);

		sgl::BitmapFont& getFont(const std::string &name);

	private:

		typedef std::map<std::string, sgl::BitmapFont*> FontMap;

		FontMap _fontMap;

	};
}

#endif
