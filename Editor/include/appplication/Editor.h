#pragma once
#include <shade/config/Shade.h>
#include <shade/core/utils/Log.h>
#include "EditorLayer.h"
//#include <shade/core/application/Application.h>

class Editor : public shade::Application
{
public:
	Editor(int argc, char* argv[]);
	virtual void	OnCreate() override;
	virtual void	OnUpdate(const shade::Timer& timer) override;
	void	OnEvent(shade::Event& e);
};