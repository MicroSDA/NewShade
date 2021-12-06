#include "Sandbox.h"

Sandbox::Sandbox(int argc, char* argv[]):
	shade::Application(argc, argv)
{
	
}

void Sandbox::OnCreate()
{
	auto& widnow = CreateWindow({ "Shade editor", 1600, 800, false });

	auto layer = this->CreateLayer<GameLayer>("GameLayer");
}

void Sandbox::OnUpdate(const shade::Timer& timer)
{
	
}

shade::Application* shade::CreateApplication(int argc, char* argv[])
{
	return new Sandbox(argc, argv);
}