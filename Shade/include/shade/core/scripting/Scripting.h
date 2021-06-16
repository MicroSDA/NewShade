#pragma once
#pragma once
//#include <Windows.h>
#include "shade/core/scripting/ScriptableEntity.h"
/* Client's sunction using for script instancing */
/* If your function getting some parameters pass it by reference */
// Args&&... when exicitable function wait &, or Args... value as argument
namespace shade
{
	template<typename T, typename ...Args>
	auto GetScript(const char* name, HMODULE& module, Args&& ...args)->T
	{
		typedef T(*script_t)(Args&& ...);
		return (T)((script_t)GetProcAddress(module, name))(std::forward<Args>(args)...);
	}
}
#ifdef SHADE_SCRIPT
	#define SHADE_SCRIPT_API __declspec(dllexport)
#else
	#define SHADE_SCRIPT_API __declspec(dllimport)
#endif
/* Prototype of function that should be created in client script */

#ifdef __cplusplus
extern "C" {
#endif
	SHADE_SCRIPT_API shade::ScriptableEntity* Instantiate(int& argc, char* argv[]);
#ifdef __cplusplus
}
#endif