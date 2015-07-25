
#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <SGL/Graphics/BitmapFont.h>
#include <SGL/Graphics/Text.h>

#include <string>
#include <map>
#include <functional>

namespace engine
{
	namespace gui
	{
		class CommandLine
		{
		public:

			typedef std::function<bool(std::vector<std::string>&)> CommandFunc;
			typedef std::vector<std::string> StringList;

			CommandLine();
			~CommandLine();

			void init();

			void addCommand(const std::string& name, const CommandFunc& func);

			void process(int keycode);

			void append(const char *str);

			void clear();

			sgl::Text& getText();

			void setActive(bool a);
			bool isActive();

		private:

			sgl::Text _text;
			std::string _buffer;

			// command map
			std::map<std::string, CommandFunc> _commands;

			bool _isActive;

		private:

			void dispatch();

			void push(const char *str);

			void removeBack();

			void tokenize(StringList& v, std::string &s, const std::string &delimiter);
		};
	}
}

#endif
