#pragma once
#include "shade/config/API.h"
#include "shade/core/scripting/Scripting.h"

namespace shade
{
	class SHADE_API Script
	{
	public:
		enum class Type
		{
			Native,
			Lua
		};

	public:
		Script(HMODULE module)
		{
			m_ScriptInstance = module;
		}
		~Script() = default;
	public:
		HMODULE m_ScriptInstance;
		ScriptableEntity* m_Entity;
	};

	class SHADE_API ScriptManager
	{
	public:
		ScriptManager() = default;
		~ScriptManager() = default;

		void Load(const char* filePath)
		{
			Script script(LoadLibraryA(filePath));
			m_Scripts[filePath] = GetScript<ScriptableEntity*>("Instantiate", script.m_ScriptInstance);
		}

		void Recompile()
		{
			
		}
	private:
		std::unordered_map<std::string, ScriptableEntity*> m_Scripts;
	};
}