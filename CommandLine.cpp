
#include "CommandLine.h"
#include "VoxelEngine.h"

namespace engine
{
	namespace gui
	{
		CommandLine::CommandLine()
		{
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
