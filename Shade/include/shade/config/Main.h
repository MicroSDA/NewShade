#pragma once
#include "shade/core/application/Application.h"

#ifdef SHADE_WINDOWS_PLATFORM

extern shade::Application* shade::CreateApplication(int argc, char* argv[]);


int main(int argc, char* argv[])
{
	
	shade::Application* pApp = nullptr;

	pApp = shade::CreateApplication(argc, argv);
	pApp->OnCreate();
	pApp->Start();

	if (pApp != nullptr)
		delete pApp;

	return 0;
}
#endif // SHADE_WINDOW_PLATFORM
