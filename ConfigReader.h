
#ifndef CONFIGREADER_H
#define CONFIGREADER_H

#include <string>
#include <cassert>

#include <boost/property_tree/ptree.hpp>

namespace engine
{
	class ConfigReader
	{
	public:

		ConfigReader();
		~ConfigReader();

		void load(const std::string& configFileName);

		bool exists(const std::string& child);

		template<typename T>
		T getValue(const std::string& child)
		{
			assert(_isLoaded);
			return _ptree.get_child("config").get<T>(child);
		}

	private:

		boost::property_tree::ptree _ptree;

		bool _isLoaded;

	};
}

#endif
