
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

		void CommandLine::dispatch()
		{
			// break the input string into tokens
			StringList tokens;
			tokenize(tokens, _buffer, " ");

			// get the command
			std::string cmd = tokens.front();

			// get command arguments
			StringList args(tokens.begin() + 1, tokens.end());

			//
			if (cmd == "SETRENDERMODE")
			{
				setRenderMode(args);
			}
			else if (cmd == "SETRENDEROPTION")
			{
				setRenderOptions(args);
			}

			// clear text
			clear();

			// disable after command is executed
			setActive(false);
		}

		void CommandLine::setRenderMode(StringList& args)
		{
			if (args.size() < 1) return;

			std::istringstream buffer(args[0]);

			int mode;
			buffer >> mode;

			VoxelEngine::getEngine()->setRenderer(mode);
		}

		void CommandLine::setRenderOptions(StringList& args)
		{
			if (args.size() < 2) return;

			std::string key   = args[0];
			std::string value = args[1];

			std::transform(key.begin(), key.end(), key.begin(), tolower);
			std::transform(value.begin(), value.end(), value.begin(), tolower);

			VoxelEngine::getEngine()->setRenderOption(key.c_str(), value.c_str());
		}

		void CommandLine::process(int keycode)
		{	
			switch (keycode)
			{
			case GLFW_KEY_ENTER:
				dispatch();
				break;

			case GLFW_KEY_BACKSPACE:
				_buffer.pop_back();
				_text.removeBack();
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
