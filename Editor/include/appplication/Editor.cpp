#include "Editor.h"

void Editor::OnCreate()
{
	m_Window = shade::Window::Create();
	m_Window->SetEventCallback(SHADE_BIND_EVENT_FN(Editor::OnEvent));


	auto& scene  = this->CreateScene("Main");

	auto camera = scene->CreateEntity("Camera");
	camera.AddComponent<shade::CameraComponent>(shade::CreateShared<shade::Camera>());

	HMODULE hModule = LoadLibrary(TEXT("../Scripts/Scripts.dll"));
	if (hModule)
	{
		auto script = shade::GetScript<shade::ScriptableEntity*>("Instantiate", hModule);
		if(script)
			camera.AddComponent<shade::NativeScriptComponent>().Bind(script);
	}

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