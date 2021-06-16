#pragma once
#include "shade/config/API.h"
#include "shade/core/scripting/ScriptableEntity.h"
#include "shade/core/scripting/Scripting.h"

namespace shade
{
	class SHADE_API NativeScript
	{
		friend class Scene;
		friend class Application;
	public:
		template<typename T>
		void Bind();
		void Bind(ScriptableEntity* instance)
		{
			InstantiateScript	= [=]() { return instance; };
			DestroyScript		= [](NativeScript* nsc) { delete nsc->m_pInstance; nsc->m_pInstance = nullptr; };
		}
	protected:
	private:
		shade::ScriptableEntity*						m_pInstance = nullptr;
		std::function<ScriptableEntity* ()>				InstantiateScript;
		void (*DestroyScript)(shade::NativeScript*)     = nullptr;
	};
	template<typename T>
	inline void NativeScript::Bind()
	{
		InstantiateScript	= []() { return static_cast<ScriptableEntity*>(new T()); };
		DestroyScript		= [](NativeScript* nsc) { delete nsc->m_pInstance; nsc->m_pInstance = nullptr; };
	}
}