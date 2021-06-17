#include "shade_pch.h"
#include "Scene.h"

shade::Scene::Scene(const std::string& name) 
	: m_Name(name)
{
}

shade::Scene::~Scene()
{
}

void shade::Scene::NativeScriptsUpdate(const shade::Timer& deltaTime)
{
	this->GetEntities().view<NativeScriptComponent>().each([=](auto entity, auto& script)
		{
			if (!script.m_pInstance)
			{
				script.m_pInstance = script.InstantiateScript();
				script.m_pInstance->m_Entity = shade::Entity{ entity, this };
				script.m_pInstance->OnCreate();
			}
			else if (script.m_pInstance->IsUpdate())
				script.m_pInstance->OnUpdate(deltaTime);
		});
}

const std::string& shade::Scene::GetName() const
{
	return m_Name;
}

shade::Entity shade::Scene::GetPrimaryCamera()
{
	this->GetEntities().view<shade::CameraComponent>().each([&](auto entity, auto& camera)
		{
			if(camera->IsPrimary())
				return Entity{ entity, this };
		});

	return Entity(); // Make use that entity is valid !
}
