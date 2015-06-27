
#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H

#include "Logger.h"

#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <luabind/operator.hpp>

#include <string>

namespace engine
{
	namespace script
	{
		class ScriptEngine
		{
		public:

			// Error Callback
			typedef void(*ErrorCallback)(const std::string& what);

			ScriptEngine();
			~ScriptEngine();

			/**
			Initialize script modules
			*/
			void init();

			/**
			Run a script
			*/
			void run(char *scriptName);

			/**

			*/
			void setErrorCallback(ErrorCallback callback);

			/**
			*/
			template<typename T>
			void addFunction(const std::string& name, T func)
			{
				using namespace luabind;

				module(_state)[
					def(name.c_str(), func)
				];
			}

			/**
			*/
			template<typename... Args>
			void callFunction(const std::string& name, Args... args)
			{
				luabind::call_function<void>(_state, name.c_str(), args...);
			}

		private:

			lua_State* _state;

			util::Logger _logger;
			ErrorCallback _errorCallback;

			/* Private Functions */

			std::string getErrorString();

		};
	}
}

#endif // SCRIPTENGINE_H
