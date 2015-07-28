
#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H

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
			void run(const std::string& scriptName);

			/**
			
			*/
			void doString(const std::string& s);

			/**
				
			*/

			/**
				Set a callback function for errors
			*/
			void setErrorCallback(ErrorCallback callback);

			/**
				Add a function to the engine
			*/
			template<typename T>
			void addFunction(const std::string& name, T func)
			{
				using namespace luabind;

				module(_state)[
					def(name.c_str(), func)
				];
			}

			void addPackagePaths();

			/**
				Call a function in the script
			*/
			template<typename... Args>
			void callFunction(const std::string& name, Args... args)
			{
				luabind::call_function<void>(_state, name.c_str(), args...);
			}

			/**
				
			*/
			static ScriptEngine& getSingleton();

		private:

			lua_State* _state;

			ErrorCallback _errorCallback;

			/* Private Functions */

			std::string getErrorString();

		};
	}
}

#endif // SCRIPTENGINE_H
