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
	m_Shader = shade::Shader::Create("BasicModel.s_shader");
	DarkVineTheme();

	vertices.resize(3);

	vertices[0].Position = glm::vec3(-0.5,  0.0, 0.0);
	vertices[1].Position = glm::vec3(-0.5, -0.5, 0.0);
	vertices[2].Position = glm::vec3( 0.0,  0.0, 0.0);


	indices.resize(3);
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	VBO = shade::VertexBuffer::Create(vertices.data(), vertices.size(), shade::VertexBuffer::BufferType::Static);
	EBO = shade::IndexBuffer::Create(indices.data(), indices.size());

	VBO->SetLayout({ {shade::VertexBuffer::ElementType::Float3, "Position"},
					 {shade::VertexBuffer::ElementType::Float2, "UV_Coords"},
					 {shade::VertexBuffer::ElementType::Float3, "Normal"},
					 {shade::VertexBuffer::ElementType::Float3, "Tangent"} });


	VAO = shade::VertexArray::Create();
	VAO->AddVertexBuffer(VBO);
	VAO->SetIndexBuffer(EBO);
}

void EditorLayer::OnUpdate(const shade::Shared<shade::Scene>& scene, const shade::Timer& deltaTime)
{

}

void EditorLayer::OnRender(const shade::Shared<shade::Scene>& scene)
{

	//if (ImGui::Begin("DockSpace", (bool*)(nullptr), m_WindowFlags))
	//{
	//	//ImGui::PopStyleVar();
	//	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	//	{
	//		ImGuiID dockspace_id = ImGui::GetID("DockSpace");
	//		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), m_DockSpaceFlags);
	//	}

	//	ShowWindowBar("Entities", &EditorLayer::Entities, this, scene.get());
	//	ShowWindowBar("Inspector", &EditorLayer::Inspector, this, m_SelectedEntity);

	//} ImGui::End(); // Begin("DockSpace")
	m_Shader->Bind();
	shade::Render::DrawIndexed(VAO);
}

void EditorLayer::OnDelete()
{

}

void EditorLayer::OnEvent(shade::Event& event)
{
	OnImGuiEvent(event);
}

void EditorLayer::Entities(shade::Scene* scene)
{
	static char search[256];
	InputText("Search", search, 256);

	if (ImGui::ListBoxHeader("##EntitiesList", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y)))
	{
		
		scene->GetEntities().view<std::string>().each([&]
		(auto entity_id, std::string& tag)
			{
				shade::Entity entity = { entity_id, scene };

				if(tag.find(search) != std::string::npos)
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


void EditorLayer::DarkVineTheme()
{
	// cherry colors, 3 intensities
#define HI(v)   ImVec4(0.502f, 0.075f, 0.256f, v)
#define MED(v)  ImVec4(0.455f, 0.198f, 0.301f, v)
#define LOW(v)  ImVec4(0.232f, 0.201f, 0.271f, v)
// backgrounds (@todo: complete with BG_MED, BG_LOW)
#define BG(v)   ImVec4(0.200f, 0.220f, 0.270f, v)
// text
#define TEXT(v) ImVec4(0.860f, 0.930f, 0.890f, v)

	auto& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Text] = TEXT(0.78f);
	style.Colors[ImGuiCol_TextDisabled] = TEXT(0.28f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
	//style.Colors[ImGuiCol_ChildWindowBg] = BG(0.58f);
	style.Colors[ImGuiCol_PopupBg] = BG(0.9f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.31f, 0.31f, 1.00f, 0.00f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = BG(1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = MED(0.78f);
	style.Colors[ImGuiCol_FrameBgActive] = MED(1.00f);
	style.Colors[ImGuiCol_TitleBg] = LOW(1.00f);
	style.Colors[ImGuiCol_TitleBgActive] = HI(1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = BG(0.75f);
	style.Colors[ImGuiCol_MenuBarBg] = BG(0.47f);
	style.Colors[ImGuiCol_ScrollbarBg] = BG(1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = MED(0.78f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = MED(1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
	style.Colors[ImGuiCol_ButtonHovered] = MED(0.86f);
	style.Colors[ImGuiCol_ButtonActive] = MED(1.00f);
	style.Colors[ImGuiCol_Header] = MED(0.76f);
	style.Colors[ImGuiCol_HeaderHovered] = MED(0.86f);
	style.Colors[ImGuiCol_HeaderActive] = HI(1.00f);
	//style.Colors[ImGuiCol_Column] = ImVec4(0.14f, 0.16f, 0.19f, 1.00f);
	//style.Colors[ImGuiCol_ColumnHovered] = MED(0.78f);
	//style.Colors[ImGuiCol_ColumnActive] = MED(1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.47f, 0.77f, 0.83f, 0.04f);
	style.Colors[ImGuiCol_ResizeGripHovered] = MED(0.78f);
	style.Colors[ImGuiCol_ResizeGripActive] = MED(1.00f);
	style.Colors[ImGuiCol_PlotLines] = TEXT(0.63f);
	style.Colors[ImGuiCol_PlotLinesHovered] = MED(1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = TEXT(0.63f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = MED(1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = MED(0.43f);
	// [...]
	//style.Colors[ImGuiCol_ModalWindowDarkening] = BG(0.73f);

	style.WindowPadding = ImVec2(6, 4);
	style.WindowRounding = 6.0f;
	style.FramePadding = ImVec2(5, 2);
	style.FrameRounding = 3.0f;
	style.ItemSpacing = ImVec2(7, 1);
	style.ItemInnerSpacing = ImVec2(1, 1);
	style.TouchExtraPadding = ImVec2(0, 0);
	style.IndentSpacing = 6.0f;
	style.ScrollbarSize = 12.0f;
	style.ScrollbarRounding = 16.0f;
	style.GrabMinSize = 20.0f;
	style.GrabRounding = 2.0f;

	style.WindowTitleAlign.x = 0.50f;

	style.Colors[ImGuiCol_Border] = ImVec4(0.539f, 0.479f, 0.255f, 0.162f);
	style.FrameBorderSize = 0.0f;
	style.WindowBorderSize = 1.0f;

}