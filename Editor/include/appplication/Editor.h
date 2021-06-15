#pragma once
#include <shade/config/Shade.h>
#include <shade/core/utils/Log.h>
//#include <shade/core/application/Application.h>

class Editor : public shade::Application
{
public:
	Editor() : shade::Application() {};
	//~Editor();
	virtual void	OnCreate() override;
};

