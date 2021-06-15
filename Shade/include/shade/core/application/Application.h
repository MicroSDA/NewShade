#pragma once
#include "shade/config/API.h"

namespace shade
{
	class SHADE_API Application
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
	private:
		static Application* m_pInstance;
	};

	Application* CreateApplication();
}