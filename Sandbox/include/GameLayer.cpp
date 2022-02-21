#include "GameLayer.h"

GameLayer::GameLayer(const std::string& name):
	shade::ImGuiLayer(name)
{
	//shade::ImGuiThemeEditor::SetColors(0x21272977, 0xFDFFFCFF, 0x621234FF, 0x13534CFF, 0x621234FF);
	shade::ImGuiThemeEditor::SetColors(0x202020FF, 0xFAFFFDFF, 0x505050FF, 0x9C1938CC, 0xFFC307B1);
	//shade::ImGuiThemeEditor::SetFont("resources/font/Roboto-Medium.ttf", 14.5);
	shade::ImGuiThemeEditor::ApplyTheme();
}

GameLayer::~GameLayer()
{
}

void GameLayer::OnCreate()
{
}

void GameLayer::OnUpdate(const shade::Shared<shade::Scene>& scene, const shade::Timer& deltaTime)
{
}

void GameLayer::OnRender(const shade::Shared<shade::Scene>& scene, const shade::Timer& deltaTime)
{
	if (ImGui::Begin("DockSpace", (bool*)(nullptr), m_WindowFlags))
	{
		//ImGui::PopStyleVar();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("DockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), m_DockSpaceFlags);
		}

		if (ImGui::Begin("Programs"))
		{
			static std::string str;
			InputText("Text 1", "Text1", str, 200);
			//ImGui::InputTextMultiline("Text", str, 1000);


			ImGui::End();
		}
		
		
	} ImGui::End(); // !Begin("DockSpace")
}

void GameLayer::OnDelete()
{

}

void GameLayer::OnEvent(const shade::Shared<shade::Scene>& scene, shade::Event& event, const shade::Timer& deltaTime)
{
}
