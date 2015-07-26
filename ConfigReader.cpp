
#include "ConfigReader.h"
#include "FatalError.h"

#include <fstream>

#include <boost/property_tree/json_parser.hpp>

namespace engine
{
	ConfigReader::ConfigReader() : _isLoaded(false)
	{
	}

	void ConfigReader::load(const std::string& configFileName)
	{
		std::ifstream file(configFileName);

		if (!file.good()) throw std::exception(std::string("Could not open file: " + configFileName).c_str());

		// read in the file
		std::stringstream buffer;
		buffer << file.rdbuf();

		// parse the json file
		boost::property_tree::ptree ptree;
		boost::property_tree::read_json(buffer, ptree);

		// check if the 'config' root node exist
		if (ptree.count("config"))
		{
			_root = ptree.get_child("config");
		}
		else
		{
			fatalError("Error: config.json must contain the attribute config as the root element");
		}
		

		_isLoaded = true;
	}

	bool ConfigReader::exists(const std::string& child)
	{
		return _isLoaded && _root.count(child) > 0;
	}

	ConfigReader::~ConfigReader()
	{
	}
}
