
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
		/**
			Provides direct user interface to the engine
		*/
		class CommandLine
		{
		public:

			typedef std::function<bool(std::vector<std::string>&)> CommandFunc;
			typedef std::vector<std::string> StringList;

			CommandLine();
			~CommandLine();

			void init();

			/**
				Add a command function to the command line
			*/
			void addCommand(const std::string& name, const CommandFunc& func);

			/**
				Process an incoming key even
			*/
			void process(int keycode);

			/**
				Append text to the command lien display

				This does not alter the buffer, only the render text
			*/
			void append(const char *str);

			/**
				Clear the command line buffer
			*/
			void clear();

			/**
				@return thr render text object
			*/
			sgl::Text& getText();

			/**
				enable the command line
			*/
			void setActive(bool a);

			/**
				Check if the command line is active
			*/
			bool isActive();

		private:

			// render text for command line
			sgl::Text _text;
			// user input string
			std::string _buffer;

			// command map
			std::map<std::string, CommandFunc> _commands;

			// command enable flag
			bool _isActive;

		private:

			/**
				Execute the input command with arguments
			*/
			void dispatch();

			/**
				push a character to the buffer
			*/
			void push(const char *str);

			/**
				remove the last character
			*/
			void removeBack();

			/**
				tokenize an input string with a delimiter
			*/
			void tokenize(StringList& v, std::string &s, const std::string &delimiter);
		};
	}
}

#endif
