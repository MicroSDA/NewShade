#include "Editor.h"

Editor::Editor(int argc, char* argv[]):
 shade::Application(argc, argv)
{
}

void Editor::OnCreate()
{
	auto& widnow = CreateWindow();
	widnow->SetEventCallback(SHADE_BIND_EVENT_FN(Editor::OnEvent));
	
	auto layer = this->CreateLayer<EditorLayer>("EditorLayer");


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
	SHADE_TRACE("Event :{0}", e.ToString());
}

shade::Application* shade::CreateApplication(int argc, char* argv[])
{
	return new Editor(argc, argv);
}