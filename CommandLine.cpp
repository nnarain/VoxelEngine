
#include "CommandLine.h"
#include "VoxelEngine.h"

#include <glfw/glfw3.h>

#include <sstream>
#include <algorithm>

namespace engine
{
	namespace gui
	{
		CommandLine::CommandLine() : _isActive(false)
		{
		}

		void CommandLine::init()
		{
			_text.setFont(&VoxelEngine::getEngine()->getResources().getFontManager().getFont("DefaultFont"));
			_text.setDimensions(20, 20);
		}

		void CommandLine::addCommand(const std::string& name, const CommandFunc& func)
		{
			_commands[name] = func;
		}

		void CommandLine::dispatch()
		{
			// break the input string into tokens
			StringList tokens;
			tokenize(tokens, _buffer, " ");

			// get the command
			std::string cmd = tokens.front();

			// get command arguments
			StringList args(tokens.begin() + 1, tokens.end());

			// get the command if it exists
			if (_commands.find(cmd) != _commands.end())
			{
				_commands[cmd](args);
			}

			// clear text
			clear();

			// disable after command is executed
			setActive(false);
		}

		void CommandLine::process(int keycode)
		{	
			static int caseOffset = (int)'a' - (int)'A';

			switch (keycode)
			{
			case GLFW_KEY_ENTER:
				dispatch();
				break;

			case GLFW_KEY_BACKSPACE:
				_buffer.pop_back();
				_text.removeBack();
				break;

			case GLFW_KEY_LEFT_SHIFT:
			case GLFW_KEY_RIGHT_SHIFT:
				break;

			default:
				std::string str = {(char)keycode};
				push(str.c_str());
				break;
			}
		}

		void CommandLine::append(const char *str)
		{
			_text << str;
		}

		void CommandLine::push(const char *str)
		{
			_text << str;
			_buffer.push_back(*str);
		}

		void CommandLine::clear()
		{
			_text.clear();
			_buffer.clear();
		}

		sgl::Text& CommandLine::getText()
		{
			return _text;
		}

		void CommandLine::setActive(bool a)
		{
			_isActive = a;

			if (!a)
			{
				clear();
			}
		}

		bool CommandLine::isActive()
		{
			return _isActive;
		}

		void CommandLine::tokenize(StringList& tokens, std::string &s, const std::string &delimiter)
		{
			// check that is still valid
			while (s.find(delimiter, 0) != std::string::npos){
				// get pos of next delimiter
				size_t pos = s.find(delimiter, 0);
				// create a token
				std::string token = s.substr(0, pos);
				// erase the current token 
				s.erase(0, pos + 1);
				// add the token to  vector
				tokens.push_back(token);
			}
			// push the last token
			tokens.push_back(s);
		}

		CommandLine::~CommandLine()
		{
		}
	}
}
