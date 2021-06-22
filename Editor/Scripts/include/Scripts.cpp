#include "Scripts.h"

extern "C"
{
	SHADE_SCRIPT_API shade::ScriptableEntity* Instantiate(int& argc, char* argv[])
	{
		return new Script();
	}
}

void Script::OnCreate()
{
}

void Script::OnDestroy()
{
}

void Script::OnUpdate(const shade::Timer& deltaTime)
{
	auto component = GetComponent<shade::CameraComponent>();
	//SHADE_TRACE("Aspect {0}", component->GetAspect());
}
