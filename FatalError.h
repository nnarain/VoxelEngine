
#ifndef FATALERROR_H
#define FATALERROR_H

#include <string>
#include <exception>

namespace engine
{
	static void fatalError(const std::string& what)
	{
		throw std::exception(what.c_str());
	}
}

#endif
