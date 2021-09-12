#include "shade_pch.h"
#include "Application.h"
#include "shade/core/layer/Layer.h"

shade::Application* shade::Application::m_pInstance = nullptr;

shade::Application::Application(int argc, char* argv[])
{
	if (!m_pInstance)
	{
		Log::Init(false, true);
		m_pInstance = this;

	}
	else
	{
		SHADE_CORE_ERROR("Application already exists!")
	}

}

shade::Application::~Application()
{
}

shade::Application& shade::Application::Get()
{
	return *m_pInstance;
}

shade::Unique<shade::Window>& shade::Application::CreateWindow(const WindowConfig& config)
{
	m_Window = Window::Create(config);
	return m_Window;
}

shade::Unique<shade::Window>& shade::Application::GetWindow()
{
	return m_Window;
}

void shade::Application::Start()
{
	if (m_Window != nullptr)
		m_Window->SetEventCallback(SHADE_BIND_EVENT_FN(Application::OnEvent));

	// TODO Scenee assigment
	Shared<Scene> dummyScene = Scene::Create();
	m_CurrentScene = Scene::Create();

	Render::Init();
	Render::SetViewPort(0, 0, m_Window->GetWidth(), m_Window->GetHeight());
	Render::SetClearColor(0.0, 0.0, 0.0, 1.0);

	while (!m_IsQuitRequested)
	{
		m_DeltaTime.Update();

		shade::Render::Clear();
		NativeScriptsUpdate(m_DeltaTime);


		((!m_CurrentScene) ? dummyScene : m_CurrentScene)->NativeScriptsUpdate(m_DeltaTime);

		for (auto& const layer : m_Layers)
		{
			if (layer->IsActive())
			{
				if (layer->IsUpdate())
					layer->OnUpdate((!m_CurrentScene) ? dummyScene : m_CurrentScene, m_DeltaTime);
				if (layer->IsRender())
				{
					layer->OnRenderBegin();
					layer->OnRender((!m_CurrentScene) ? dummyScene : m_CurrentScene, m_DeltaTime);
					layer->OnRenderEnd();
				}
			}
		}

		m_Window->OnUpdate();
		AssetManager::_Get()._DispatchAssets();
	}

	shade::Render::ShutDown();
	shade::AssetManager::_Get()._Clear();
}

void shade::Application::Quit()
{
	m_IsQuitRequested = true;
}

void shade::Application::OnEvent(shade::Event& event)
{
	if (event.GetEventType() == EventType::WindowClose)
		m_IsQuitRequested = true;

	for (auto& layer : m_Layers)
	{
		if (layer->IsActive())
			layer->OnEvent(m_CurrentScene, event, m_DeltaTime);
	}

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
	//m_CurrentScene = shade::CreateShared<Scene>(name);
	//return m_CurrentScene;
	return nullptr;
}

void shade::Application::DeleteCurrentScene()
{
	m_CurrentScene.reset();
}

void shade::Application::SetCurrentScene(const Shared<Scene>& scene)
{
	m_CurrentScene = scene;
}

shade::Shared<shade::Scene> shade::Application::GetCurrentScene()
{
	return m_CurrentScene;
}

bool shade::Application::DeleteLayer(const std::string& name)
{
	for (auto& layer : m_Layers)
	{
		if (layer->GetName() == name)
		{
			delete layer;
			return true;
		}

	}

	SHADE_CORE_WARNING("Layer '{0}' has not been found!", name);
	return false;
}
