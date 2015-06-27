
#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <string>

namespace engine
{
	namespace util
	{
		class Logger
		{
		public:

			Logger(const std::string& logName);
			~Logger();

			void log(const std::string& what);

		private:

			std::ofstream _logFile;
		};
	}
}

#endif
