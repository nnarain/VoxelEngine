
#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <SGL/Graphics/BitmapFont.h>
#include <SGL/Graphics/Text.h>

#include <sstream>
#include <string>

namespace engine
{
	namespace gui
	{
		class CommandLine
		{
		public:
			CommandLine();
			~CommandLine();

			void init();

			void append(const char *str);

			sgl::Text& getText();

		private:

			sgl::Text _text;

			std::stringstream _buffer;

		};
	}
}

#endif
