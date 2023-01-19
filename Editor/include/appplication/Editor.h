#pragma once
#include <shade/config/Shade.h>
#include <shade/core/utils/Log.h>
#include "EditorLayer.h"
//#include <shade/core/application/Application.h>
// Forse use discret grpahics, need to fand another good place for it
extern "C"
{
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

class Editor : public shade::Application
{
public:
	Editor(int argc, char* argv[]);
	virtual void	OnCreate() override;
	virtual void	OnUpdate(const shade::Timer& timer) override;
};