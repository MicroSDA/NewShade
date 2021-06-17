#pragma once
#include "shade/config/API.h"
#include "shade/core/events/ApplicationEvent.h"
#include "shade/core/window/Window.h"
#include "shade/core/entities/EntitiesDocker.h"
#include "shade/core/entities/Entity.h"
#include "shade/core/time/Timer.h"
#include "shade/core/components/Components.h"
#include "shade/core/scene/Scene.h"
#include "shade/core/layer/Layer.h"

namespace shade
{
	class SHADE_API Application : public EntitiesDocker
	{
	public:
		Application();
		virtual ~Application(); 
		static  Application& Get();
	public:
	//Need make private and specific friend for handle this
		void Start();
		void Quit();
		virtual void	OnCreate() = 0;
		void			OnEvent(shade::Event& e);
		virtual void    OnUpdate(const shade::Timer& timer) = 0;
		void            NativeScriptsUpdate(const shade::Timer& timer);
	protected:
		Unique<Window>				m_Window;
		Shared<Scene>				m_CurrentScene;
		Shared<Scene>				CreateScene(const std::string& name);
		Layer*						GetLayer(const std::string& name);
		void						DeleteCurrentScene();
		template<typename T>
		T*			                CreateLayer(const std::string& name);
		template<typename T>
		auto						GetLayer(const std::string& name)->T*;
	private:
		static Application*			m_pInstance;
		bool						m_IsQuitRequested = false;
		std::vector<Layer*>	        m_Layers;
	};

	Application* CreateApplication();
	template<typename T>
	inline T* Application::CreateLayer(const std::string& name)
	{
		for (auto& layer : m_Layers)
		{
			if (layer->GetName() == name)
			{
				SHADE_CORE_WARNING("Layer '{0}' allready exist!", name);
				return nullptr;
			}

		}

		auto pLayer = new T(name);
		pLayer->OnCreate();
		m_Layers.push_back(pLayer);
		return pLayer;
	}
	template<typename T>
	inline auto Application::GetLayer(const std::string& name) -> T*
	{
		for (auto& layer : m_Layers)
		{
			if (layer->GetName() == name)
				return static_cast<T*>(layer);
		}
		
		SHADE_CORE_WARNING("Layer '{0}' has not been found!", name);
		return nullptr;
	}
}