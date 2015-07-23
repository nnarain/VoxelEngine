
#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <SGL/Graphics/BitmapFont.h>
#include <SGL/Graphics/Text.h>

#include <string>

typedef std::vector<std::string> StringList;

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

			void process(int keycode);

			void append(const char *str);

			void clear();

			sgl::Text& getText();

			void setActive(bool a);
			bool isActive();

		private:

			sgl::Text _text;
			std::string _buffer;

			bool _isActive;

		private:

			void dispatch();

			void setRenderMode(StringList& args);
			void setRenderOptions(StringList& args);

			void push(const char *str);

			void tokenize(StringList& v, std::string &s, const std::string &delimiter);
		};
	}
}

#endif
