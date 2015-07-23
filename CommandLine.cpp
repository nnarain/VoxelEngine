
#include "CommandLine.h"
#include "VoxelEngine.h"

namespace engine
{
	namespace gui
	{
		CommandLine::CommandLine()
		{
		}

		void CommandLine::init()
		{
			_text.setFont(&VoxelEngine::getEngine()->getResources().getFontManager().getFont("DefaultFont"));
		}

		void CommandLine::append(const char *str)
		{
			_text << str;
		}

		sgl::Text& CommandLine::getText()
		{
			return _text;
		}

		CommandLine::~CommandLine()
		{
		}
	}
}
