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
	Timer deltaTime;
	Shared<Scene> dummyScene = CreateUnique<Scene>("Dummy");

	while (!m_IsQuitRequested)
	{
		deltaTime.Update();

		NativeScriptsUpdate(deltaTime);

		if (m_CurrentScene != nullptr)
		{
			m_CurrentScene->NativeScriptsUpdate(deltaTime);

			for (auto&const layer : m_Layers)
			{
				if (layer->IsActive())
				{
					if (layer->IsUpdate())
						layer->OnUpdate(m_CurrentScene, deltaTime);
					if (layer->IsRender())
					{
						layer->OnRenderBegin();
						layer->OnRender(m_CurrentScene);
						layer->OnRenderEnd();
					}
				}
			}
		}
		else
		{
			for (auto& const layer : m_Layers)
			{
				if (layer->IsActive())
				{
					if (layer->IsUpdate())
						layer->OnUpdate(dummyScene, deltaTime);
					if (layer->IsRender())
					{
						layer->OnRenderBegin();
						layer->OnRender(dummyScene);
						layer->OnRenderEnd();
					}
				}
			}
		}
		//
		glClearColor(1, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		m_Window->OnUpdate();
	}
}

void shade::Application::Quit()
{
	m_IsQuitRequested = true;
}

void shade::Application::OnEvent(shade::Event& e)
{

}
shade::Layer* shade::Application::GetLayer(const std::string& name) 
{
	for (auto& layer : m_Layers)
	{
		if (layer->GetName() == name)
			return layer;
	}

	SHADE_CORE_WARNING("Layer '{0}' has not been found!", name);
	return nullptr;
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

shade::Shared<shade::Scene> shade::Application::CreateScene(const std::string& name)
{
	m_CurrentScene = shade::CreateShared<Scene>(name);
	return m_CurrentScene;
}

void shade::Application::DeleteCurrentScene()
{
	m_CurrentScene.reset();
}
