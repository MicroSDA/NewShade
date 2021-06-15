#include "shade_pch.h"
#include "Application.h"

shade::Application* shade::Application::m_pInstance = nullptr;

shade::Application::Application()
{
	//if(!m_pInstance)
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
}

void shade::Application::Quit()
{
}