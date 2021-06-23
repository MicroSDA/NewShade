#include "EditorLayer.h"

EditorLayer::EditorLayer(const std::string& name) :
	shade::ImGuiLayer(name)
{
}

EditorLayer::~EditorLayer()
{
}

void EditorLayer::OnCreate()
{

}

void EditorLayer::OnUpdate(const shade::Shared<shade::Scene>& scene, const shade::Timer& deltaTime)
{

}

/**void EditorLayer::OnRenderBegin()
{

*/

void EditorLayer::OnRender(const shade::Shared<shade::Scene>& scene)
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

		ShowWindowBar("Entities", &EditorLayer::Entities, this, scene.get());
		ShowWindowBar("Inspector", &EditorLayer::Inspector, this, m_SelectedEntity);


	} ImGui::End(); // Begin("DockSpace")
}

/*void EditorLayer::OnRenderEnd()
{

}*/

void EditorLayer::OnDelete()
{

}

void EditorLayer::Entities(shade::Scene* scene)
{

	if (ImGui::ListBoxHeader("##EntitiesList", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y)))
	{
		scene->GetEntities().view<std::string>().each([&]
		(auto entity_id, std::string& tag)
			{
				shade::Entity entity = { entity_id, scene };
				if (ImGui::Selectable(tag.c_str(), m_SelectedEntity == entity))
					m_SelectedEntity = entity;
			});

		ImGui::ListBoxFooter();
	}
}

void EditorLayer::Inspector(shade::Entity& entity)
{
	DrawComponent<std::string>("Tag", entity, &EditorLayer::TagComponent, this, entity); // "this" as first argument ! for std::invoke when non static fucntion provided it should know instance
}

void EditorLayer::TagComponent(shade::Entity& entity)
{
	auto& tag = entity.GetComponent<shade::Tag>();
	ImGui::Text(tag.c_str());
}

void EditorLayer::Transform3DComponent(shade::Entity& entity)
{
	auto& tag = entity.GetComponent<int>();
	ImGui::Text(std::to_string(tag).c_str());
}
