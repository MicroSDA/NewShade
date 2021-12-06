#pragma once
#include <shade/config/Shade.h>
#include <shade/core/utils/Log.h>
#include "GameLayer.h"

class Sandbox : public shade::Application
{
public:
	Sandbox(int argc, char* argv[]);
	virtual void	OnCreate() override;
	virtual void	OnUpdate(const shade::Timer& timer) override;
};