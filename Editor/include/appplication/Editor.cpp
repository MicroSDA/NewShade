#include "Editor.h"

Editor::Editor(int argc, char* argv[]):
 shade::Application(argc, argv)
{
}

void Editor::OnCreate()
{
	auto& widnow = CreateWindow();
	shade::AssetManager::LoadAssetDataListFromFile("resources/assets/Assets.xml");
	auto& scene = this->CreateScene("Main");


	auto layer = this->CreateLayer<EditorLayer>("EditorLayer");
	auto camera = CreateEntity("Camera");


	camera.AddComponent<shade::CameraComponent>(shade::CreateShared<shade::Camera>());

	HMODULE hModule = LoadLibrary(TEXT("../Scripts/Scripts.dll"));
	if (hModule)
	{
		auto script = shade::GetScript<shade::ScriptableEntity*>("Instantiate", hModule);
		if(script)
			camera.AddComponent<shade::NativeScriptComponent>().Bind(script);
	}

	//auto directLight = scene->CreateEntity("DirectLight");
	//directLight.AddComponent<shade::EnvironmentComponent>(shade::CreateShared<shade::DirectLight>());
}

void Editor::OnUpdate(const shade::Timer& timer)
{
	
}

shade::Application* shade::CreateApplication(int argc, char* argv[])
{
	return new Editor(argc, argv);
}