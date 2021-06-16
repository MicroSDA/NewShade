#include "Editor.h"

void Editor::OnCreate()
{
	m_Window = shade::Window::Create();
	m_Window->SetEventCallback(SHADE_BIND_EVENT_FN(Editor::OnEvent));

	
	auto cube = this->CreateEntity("Test");


		
	auto& script = cube.AddComponent<shade::NativeScriptComponent>();

	HMODULE hModule = LoadLibrary(TEXT("../Scripts/Scripts.dll"));

	auto s = shade::GetScript<shade::ScriptableEntity*>("Instantiate", hModule);

	script.Bind(shade::GetScript<shade::ScriptableEntity*>("Instantiate", hModule));
	cube.AddComponent<shade::TestComponent>(shade::CreateShared<std::string>(std::string("Hello")));

	//cube.AddComponent<std::string>("Hello");
	//cube.AddComponent<std::string>("asd");
}

void Editor::OnUpdate(const shade::Timer& timer)
{
	
}

void Editor::OnEvent(shade::Event& e)
{
	
}

shade::Application* shade::CreateApplication()
{
	return new Editor();
}