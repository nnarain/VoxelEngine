
#ifndef CONFIGREADER_H
#define CONFIGREADER_H

#include <string>
#include <cassert>

#include <boost/property_tree/ptree.hpp>

namespace engine
{
	/**
		Read a JSON configuration file
	*/
	class ConfigReader
	{
	public:

		ConfigReader();
		~ConfigReader();

		/**
			Load the config file
		*/
		void load(const std::string& configFileName);

		/**
			Check if a value exists under the "config" root node
		*/
		bool exists(const std::string& child);

		/**
			Get a generic value from the config file under the "config" root node
		*/
		template<typename T>
		T getValue(const std::string& child)
		{
			assert(_isLoaded);
			return _root.get<T>(child);
		}

	private:

		boost::property_tree::ptree _root;

		bool _isLoaded;

	};
}

#endif
