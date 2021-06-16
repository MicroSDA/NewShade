#include "shade_pch.h"
#include "Application.h"
#include <glad/glad.h>

shade::Application* shade::Application::m_pInstance = nullptr;

shade::Application::Application()
{
	Log::Init();
	
}

shade::Application::~Application()
{
}

shade::Application& shade::Application::Get()
{
	return *m_pInstance;
}

void shade::Application::Start()
{
	shade::Timer deltaTime;
	while (!m_IsQuitRequested)
	{
		deltaTime.Update();
		NativeScriptsUpdate(deltaTime);

		glClearColor(1, 0, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		m_Window->OnUpdate();
	}
}

void shade::Application::Quit()
{
}

void shade::Application::OnEvent(shade::Event& e)
{

}

void shade::Application::NativeScriptsUpdate(const shade::Timer& timer)
{
	this->GetEntities().view<shade::NativeScriptComponent>().each([=](auto entity, auto& script)
		{
			if (!script.m_pInstance)
			{
				script.m_pInstance = script.InstantiateScript();
				script.m_pInstance->m_Entity = shade::Entity{ entity, this };
				script.m_pInstance->OnCreate();
			}
			else if (script.m_pInstance->IsUpdate())
				script.m_pInstance->OnUpdate(timer);
		});
}
