#include "Editor.h"

Editor::Editor(int argc, char* argv[]):
 shade::Application(argc, argv)
{
}

void Editor::OnCreate()
{
	auto& widnow = CreateWindow();
	shade::AssetManager::LoadAssetDataListFromFile("resources/assets/Assets.xml");
	//auto& scene = this->CreateScene("Main");


	auto layer = this->CreateLayer<EditorLayer>("EditorLayer");
	auto camera = CreateEntity("Camera");


	auto& _camera = camera.AddComponent<shade::CameraComponent>(shade::CreateShared<shade::Camera>());
	_camera->SetDirection(glm::vec3(-0.011512465, -0.33462766, 0.94228005));
	_camera->SetPosition(glm::vec3(0, 10, -20));
	// x:-0.011512465 y:-0.33462766 z:0.94228005
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