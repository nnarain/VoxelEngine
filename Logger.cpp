
#include "Logger.h"

using namespace engine::util;

Logger::Logger(const std::string& logName) : _logFile(logName)
{
}

void Logger::log(const std::string& what)
{
	_logFile << what << std::endl;
}

Logger::~Logger()
{
}
