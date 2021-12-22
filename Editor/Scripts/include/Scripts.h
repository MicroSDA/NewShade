#pragma once
#define SHADE_SCRIPT
//#define SHADE_BUILD_DLL

#include <shade/core/scripting/Scripting.h>
#include <shade/core/components/Components.h>

class EditorCameraScript : public shade::ScriptableEntity
{
public:
	void OnCreate();
	void OnDestroy();
	void OnUpdate(const shade::Timer& deltaTime);
private:
	float m_RotationSpeed = 0.3f;
	float m_MovementSpeed = 0.05f;
};