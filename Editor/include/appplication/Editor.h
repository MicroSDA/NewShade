#pragma once
#include <shade/config/Shade.h>
#include <shade/core/utils/Log.h>
//#include <shade/core/application/Application.h>

class Editor : public shade::Application
{
public:
	virtual void	OnCreate() override;
	virtual void	OnUpdate(const shade::Timer& timer) override;
	void	OnEvent(shade::Event& e);
};