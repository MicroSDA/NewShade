#pragma once
#include "shade/config/API.h"
#include "shade/core/events/ApplicationEvent.h"
#include "shade/core/window/Window.h"
#include "shade/core/entities/EntitiesDocker.h"
#include "shade/core/entities/Entity.h"
#include "shade/core/time/Timer.h"
#include "shade/core/components/Components.h"

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
	private:
		static Application* m_pInstance;
		bool   m_IsQuitRequested = false;
	protected:
		Unique<Window> m_Window;
	};

	Application* CreateApplication();
}