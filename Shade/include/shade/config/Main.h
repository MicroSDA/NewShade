#pragma once
#include "shade/core/application/Application.h"

#ifdef SHADE_WINDOWS_PLATFORM

extern shade::Application* shade::CreateApplication();

int main()
{
	shade::Application* pApp = nullptr;

	pApp = shade::CreateApplication();
	pApp->OnCreate();
	pApp->Start();

	if (pApp != nullptr)
		delete pApp;

	return 0;
}
#endif // SHADE_WINDOW_PLATFORM
