#include "Editor.h"

Editor::Editor(int argc, char* argv[]):
 shade::Application(argc, argv)
{
}

void Editor::OnCreate()
{
	//Should be first before tuching opengl functions
	auto& widnow = CreateWindow({"Shade editor", 1600, 800, false });
	// Create shaders 

	shade::ShadersLibrary::Create("Bloom",		        "resources/shaders/PostProcess/Bloom/Bloom.glsl");
	shade::ShadersLibrary::Create("ColorCorrection",    "resources/shaders/PostProcess/ColorCorrection/ColorCorrection.glsl");
	shade::ShadersLibrary::Create("Sprite",				"resources/shaders/2D/Sprite.glsl");
	

	// Load assets data list
	shade::AssetManager::LoadAssetDataListFromFile("resources/assets/Assets.xml");

	auto camera = CreateEntity("Camera");// TODO need to remove from there some how
	auto& _camera = camera.AddComponent<shade::CameraComponent>(shade::CreateShared<shade::Camera>());
	_camera->SetDirection(glm::vec3(-0.011512465, -0.33462766, 0.94228005));
	_camera->SetPosition(glm::vec3(0, 10, -20));

	HMODULE hModule = LoadLibrary(TEXT("../Scripts/Scripts.dll"));

	char* name = { "EditorCamera" };

	if (hModule)
	{
		auto script = shade::GetScript<shade::ScriptableEntity*>("Instantiate", hModule, name);
		if(script)
			camera.AddComponent<shade::NativeScriptComponent>().Bind(script);
	}

	//FreeLibrary(hModule);

	auto layer = this->CreateLayer<EditorLayer>("EditorLayer");

}

void Editor::OnUpdate(const shade::Timer& timer)
{
	
}

shade::Application* shade::CreateApplication(int argc, char* argv[])
{
	return new Editor(argc, argv);
}