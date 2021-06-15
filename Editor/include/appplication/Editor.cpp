#include "Editor.h"

shade::Application* shade::CreateApplication()
{
	return new Editor();
}

void Editor::OnCreate()
{
	shade::Log::Init();
	SHADE_INFO("Hello world")
}