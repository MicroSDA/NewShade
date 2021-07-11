#include "EditorLayer.h"
//#include <pugixml/pugixml.hpp> // Temporary
//#include <pugixml/pugiconfig.hpp> // Temporary

EditorLayer::EditorLayer(const std::string& name) :
	shade::ImGuiLayer(name)
{
}

EditorLayer::~EditorLayer()
{
}

void EditorLayer::OnCreate()
{
	m_FrameBuffer = shade::FrameBuffer::Create(shade::FrameBuffer::Layout(100, 100, { shade::FrameBuffer::Texture::Format::RGBA8,shade::FrameBuffer::Texture::Format::DEPTH24STENCIL8 }));
	m_InstancedShader = shade::Shader::Create("resources/shaders/BasicModel.glsl");
	m_GridShader = shade::Shader::Create("resources/shaders/Grid.glsl");

	m_Grid = shade::Grid::Create(0, 0, 0);
	DarkVineTheme();

	
}

void EditorLayer::OnUpdate(const shade::Shared<shade::Scene>& scene, const shade::Timer& deltaTime)
{
	// When scene is playing
	/*scene->GetEntities().view<shade::CameraComponent>().each([&](auto entity, auto& camera)
		{
			m_EditorCamera = camera;
		});*/
	// When scene inst playing
	shade::Application::Get().GetEntities().view<shade::CameraComponent>().each([&](auto entity, auto& camera) { m_EditorCamera = camera; });

	auto& instancedPool = scene->GetEntities().view<shade::Model3DComponent, shade::Transform3DComponent>();
	instancedPool.each( [&] (auto& entity, shade::Model3DComponent& model, shade::Transform3DComponent& transform)
		{
			for (auto& mesh : model->GetMeshes())
			{
				shade::Render::SubmitInstanced(mesh.get(), transform.GetModelMatrix(), mesh->GetMaterial(), mesh->GetTextures(), instancedPool.size());
			}
		});

	shade::Render::Submit(m_Grid.get(), glm::mat4(1));
}

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
		ShowWindowBar("Scene", &EditorLayer::Scene, this, scene.get());

	} ImGui::End(); // Begin("DockSpace")

	{
		auto environments		= scene->GetEntities().view<shade::EnvironmentComponent>();
		shade::Render::Begin(m_FrameBuffer);
			
			shade::Render::BeginScene(m_GridShader, m_EditorCamera);
				shade::Render::DepthTest(false);
				shade::Render::DrawSubmited(m_GridShader);
			shade::Render::EndScene(m_GridShader);

			shade::Render::BeginScene(m_InstancedShader, m_EditorCamera, environments.raw(), environments.size());
				shade::Render::DepthTest(true);
				shade::Render::DrawInstanced(m_InstancedShader);
			shade::Render::EndScene(m_InstancedShader);

		shade::Render::End(m_FrameBuffer);
	}
}

void EditorLayer::OnDelete()
{

}

void EditorLayer::OnEvent(const shade::Shared<shade::Scene>& scene, shade::Event& event)
{
	OnImGuiEvent(event);
	if (event.GetEventType() == shade::EventType::KeyPressed)
	{
		shade::KeyCode keyCode = static_cast<shade::KeyEvent*>(&event)->GetKeyCode();

		if (keyCode == shade::Key::Insert)
		{
			for (auto i = 0; i < 100; i++)
			{
				shade::AssetManager::Hold<shade::Model3D>("nanosuit", shade::Asset::State::RemoveIfPosible,
					[scene](auto& model)
					{
						auto& entity = scene->CreateEntity("Nanosuit");
						entity.AddComponent<shade::Model3DComponent>(shade::AssetManager::Receive<shade::Model3D>(model));
						auto& transform = entity.AddComponent<shade::Transform3DComponent>();
						transform.SetPostition(1 + rand() % 100, 1 + rand() % 100, 1 + rand() % 100);
						transform.SetRotation(1 + rand() % 100, 1 + rand() % 100, 1 + rand() % 100);
					});
				shade::AssetManager::Hold<shade::Model3D>("Cube", shade::Asset::State::RemoveIfPosible,
					[scene](auto& model)
					{
						auto& entity = scene->CreateEntity("Cube");
						entity.AddComponent<shade::Model3DComponent>(shade::AssetManager::Receive<shade::Model3D>(model));
						auto& transform = entity.AddComponent<shade::Transform3DComponent>();
						transform.SetPostition(1 + rand() % 100, 1 + rand() % 100, 1 + rand() % 100);
						transform.SetRotation(1 + rand() % 100, 1 + rand() % 100, 1 + rand() % 100);
					});
			}
		}
		else if (keyCode == shade::Key::Home)
		{
			scene->DestroyEntities();
		}
		else if (m_SelectedEntity.IsValid() && keyCode == shade::Key::F)
		{
			switch (m_GuizmoOperation)
			{
			case ImGuizmo::OPERATION::TRANSLATE: 
				if(m_AllowedGuizmoOperation & ImGuizmo::OPERATION::ROTATE)
					m_GuizmoOperation = ImGuizmo::OPERATION::ROTATE; break;
			case ImGuizmo::OPERATION::ROTATE:
				if (m_AllowedGuizmoOperation & ImGuizmo::OPERATION::SCALE)
					m_GuizmoOperation = ImGuizmo::OPERATION::SCALE; break;
			case ImGuizmo::OPERATION::SCALE:
				if (m_AllowedGuizmoOperation & ImGuizmo::OPERATION::TRANSLATE)
					m_GuizmoOperation = ImGuizmo::OPERATION::TRANSLATE; break;
			}
		}
		
	}
}

void EditorLayer::Entities(shade::Scene* scene)
{
	static char search[256];
	InputText("Search", search, 256);

	
	if (ImGui::ListBoxHeader("##EntitiesList", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y)))
	{
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("New entity"))
			{
				scene->CreateEntity("New entity");
			}
			if (ImGui::BeginMenu("Lighting"))
			{
				if (ImGui::MenuItem("Add dirrect light"))
				{
					scene->CreateEntity("Direct light").AddComponent<shade::EnvironmentComponent>(shade::CreateShared<shade::DirectLight>());
				}
				if (ImGui::MenuItem("Add point light"))
				{
					scene->CreateEntity("Point light").AddComponent<shade::EnvironmentComponent>(shade::CreateShared<shade::PointLight>());
				}
				if (ImGui::MenuItem("Add spot light"))
				{
					scene->CreateEntity("Spot light").AddComponent<shade::EnvironmentComponent>(shade::CreateShared<shade::SpotLight>());
				}

				ImGui::EndMenu();
			}

			ImGui::EndPopup();
		}
		scene->GetEntities().view<std::string>().each([&]
		(auto entity_id, std::string& tag)
			{
				shade::Entity	entity = { entity_id, scene };
				std::string		_tag = tag + "##" + entity;
				if (tag.find(search) != std::string::npos)
					if (ImGui::Selectable(_tag.c_str(), m_SelectedEntity == entity))
						m_SelectedEntity = entity;
			});

		ImGui::ListBoxFooter();

		if (m_SelectedEntity.IsValid())
		{
			if (ImGui::BeginPopupContextItem("##ComponentPopup"))
			{
				
				if (!m_SelectedEntity.HasComponent<shade::EnvironmentComponent>() && !m_SelectedEntity.HasComponent<shade::CameraComponent>())
				{
					AddComponent<shade::Transform3DComponent>("Transform3D", false, m_SelectedEntity, [](shade::Entity& entity) 
						{
							entity.AddComponent<shade::Transform3DComponent>();

						},m_SelectedEntity);

					AddComponent<shade::Model3DComponent>("Model3D", true, m_SelectedEntity, [&](shade::Entity& entity) 
						{
							for (auto& asset : shade::AssetManager::GetAssetsDataList())
							{
								if (strcmp(asset.second.Attribute("type").as_string(), "model3D") == 0)
									if (ImGui::MenuItem(asset.first.c_str()))
									{
										shade::AssetManager::Hold<shade::Model3D>(asset.first, shade::Asset::State::RemoveIfPosible,
											[&](auto& model)
											{
												m_SelectedEntity.AddComponent<shade::Model3DComponent>(shade::AssetManager::Receive<shade::Model3D>(model));
											});

									}
							}
						}, m_SelectedEntity);	
				}

				if (!m_SelectedEntity.HasComponent<shade::Transform3DComponent>() &&
					!m_SelectedEntity.HasComponent<shade::Model3DComponent>() &&
					!m_SelectedEntity.HasComponent<shade::CameraComponent>())
				{
					/*AddComponent<shade::EnvironmentComponent>("DirectLight", m_SelectedEntity, &EditorLayer::AddDirectLight);
					AddComponent<shade::EnvironmentComponent>("PointLight", m_SelectedEntity, &EditorLayer::AddPointLight);
					AddComponent<shade::EnvironmentComponent>("SpotLight", m_SelectedEntity, &EditorLayer::AddSpotLight);*/
				}
				// Remove, need to create specific fucntion ?
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7, 0, 0, 1));
				if (ImGui::MenuItem("Remove entity"))
					m_SelectedEntity.Destroy();
				ImGui::PopStyleColor();
				ImGui::EndPopup();
			}
		}
	}
}

void EditorLayer::Inspector(shade::Entity& entity)
{
	DrawComponent<shade::Tag>("Tag", entity,       &EditorLayer::TagComponent, this, entity); // "this" as first argument ! for std::invoke when non static fucntion provided it should know instance
	DrawComponent<shade::Transform3DComponent>("Transform", entity, &EditorLayer::Transform3DComponent, this, entity); // "this" as first argument ! for std::invoke when non static fucntion provided it should know instance
	DrawComponent<shade::Model3DComponent>("Model", entity, &EditorLayer::Model3dComponent, this, entity); // "this" as first argument ! for std::invoke when non static fucntion provided it should know instance
	DrawComponent<shade::EnvironmentComponent>("Enviroment", entity, &EditorLayer::EnvironmentComponent, this, entity); // "this" as first argument ! for std::invoke when non static fucntion provided it should know instance
}

void EditorLayer::Scene(shade::Scene* scene)
{
	if (m_SceneViewPort.x != ImGui::GetContentRegionAvail().x || m_SceneViewPort.y != ImGui::GetContentRegionAvail().y)
	{
		m_SceneViewPort = ImGui::GetContentRegionAvail();
		m_FrameBuffer->Resize(m_SceneViewPort.x, m_SceneViewPort.y);
		m_EditorCamera->Resize(m_SceneViewPort.x / m_SceneViewPort.y);
	}
	else
	{

		ImGui::Image(reinterpret_cast<void*>(m_FrameBuffer->GetAttachment(0)),
			m_SceneViewPort, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		FpsOverlay(ImGui::GetWindowViewport());

		if (m_SelectedEntity.IsValid())
		{
			if (m_SelectedEntity.HasComponent<shade::Transform3DComponent>())
			{
				m_AllowedGuizmoOperation = m_BasicGuizmoOperation;
				auto transform = m_SelectedEntity.GetComponent<shade::Transform3DComponent>().GetModelMatrix();

				if (DrawImGuizmo(transform, m_EditorCamera, m_GuizmoOperation, ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowSize().x, ImGui::GetWindowSize().y))
				{
					glm::vec3 position, rotation, scale;
					ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transform), glm::value_ptr(position), glm::value_ptr(rotation), glm::value_ptr(scale));
					m_SelectedEntity.GetComponent<shade::Transform3DComponent>().SetPostition(position);
					m_SelectedEntity.GetComponent<shade::Transform3DComponent>().SetRotation(glm::radians(rotation));
					m_SelectedEntity.GetComponent<shade::Transform3DComponent>().SetScale(scale);
				}
			}
			else if (m_SelectedEntity.HasComponent<shade::EnvironmentComponent>())
			{
				auto& env = m_SelectedEntity.GetComponent<shade::EnvironmentComponent>();
				switch (env->GetType())
				{
				case shade::Environment::Type::DirectLight:
				{
					m_AllowedGuizmoOperation = m_DirectLightGuizmoOperation;
					m_GuizmoOperation = ImGuizmo::OPERATION::ROTATE;
					auto light = static_cast<shade::DirectLight*>(env.get());
					glm::mat4 transform = glm::toMat4(glm::quat((light->GetDirection())));
					if (DrawImGuizmo(transform, m_EditorCamera, m_GuizmoOperation, ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowSize().x, ImGui::GetWindowSize().y))
					{
						glm::vec3 position, rotation, scale;
						ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transform), glm::value_ptr(position), glm::value_ptr(rotation), glm::value_ptr(scale));
						light->SetDirection(glm::radians(rotation)); // TODO: Normalize
					}
					break;
				}
				default:
					break;
				}
				;
			}
		}
	}
}

void EditorLayer::TagComponent(shade::Entity& entity)
{
	static char buffer[256]; memset(buffer, 0, sizeof(buffer));
	auto& tag = entity.GetComponent<shade::Tag>();
	std::strncpy(buffer, tag.c_str(), sizeof(buffer));

	if (ImGui::InputTextWithHint("##Tag", tag.c_str(), buffer, sizeof(buffer)))
		tag = std::string(buffer);
}

void EditorLayer::Transform3DComponent(shade::Entity& entity)
{
	auto& transform = entity.GetComponent<shade::Transform3DComponent>();
	DrawVec3F("Position", glm::value_ptr(transform.GetPosition()));
	DrawVec3F("Roatation", glm::value_ptr(transform.GetRotation()));
	DrawVec3F("Scale", glm::value_ptr(transform.GetScale()), 1.0f);
}

void EditorLayer::Model3dComponent(shade::Entity& entity)
{
	if (ImGui::TreeNodeEx("Meshes: ", ImGuiTreeNodeFlags_KeppFramedWhenOpen))
	{
		auto& model = entity.GetComponent<shade::Model3DComponent>();

		for (auto i = 0; i < model->GetMeshes().size(); i++)
		{
			// When model is imported, asset data is nullptr and we have crash in the GetAssetData()
			DrawTreeNode(model->GetMeshes()[i]->GetAssetData().Attribute("id").as_string(),
				[&](const shade::Shared<shade::Mesh>& mesh)
				{
					DrawTreeNode("Material", [&](shade::Material& material) 
						{
							DrawColor3("Ambinet",  glm::value_ptr(material.GetAmbientColor()));
							DrawColor3("Diffuse",  glm::value_ptr(material.GetDiffuseColor()));
							DrawColor3("Specular", glm::value_ptr(material.GetSpecularColor()));
							DrawFlaot("Shininess",	   &material.GetShininess());
							DrawFlaot("Shininess strength",	   &material.GetShininessStrength());

						}, mesh->GetMaterial());

					DrawTreeNode("Textures", [&](const std::vector<shade::Shared<shade::Texture>>& textures)
						{
							for (auto texture : mesh->GetTextures())
							{
								ImGui::Text(texture->GetAssetData().Attribute("texture_type").as_string());
								DrawImage(texture->GetRenderID(), 100, 100, true);
							}
						}, mesh->GetTextures());

				}, model->GetMeshes()[i]);
		}

		ImGui::TreePop();
	}
}
void EditorLayer::EnvironmentComponent(shade::Entity& entity)
{
	auto& environment = entity.GetComponent<shade::EnvironmentComponent>();
	switch (environment->GetType())
	{
	case shade::Environment::Type::DirectLight:
	{
		auto light = static_cast<shade::DirectLight*>(environment.get());
		DrawVec3F("Direction", glm::value_ptr(light->GetDirection()));
		DrawColor3("Ambient",   glm::value_ptr(light->GetAmbientColor()));
		DrawColor3("Diffuse", glm::value_ptr(light->GetDiffuseColor()));
		DrawColor3("Specular", glm::value_ptr(light->GetSpecularColor()));

		break;
	}
	case shade::Environment::Type::PointLight:
	{
		auto light = static_cast<shade::PointLight*>(environment.get());
		DrawVec3F("Position",	glm::value_ptr(light->GetPosition()));
		DrawColor3("Ambient",	glm::value_ptr(light->GetAmbientColor()));
		DrawColor3("Diffuse",	glm::value_ptr(light->GetDiffuseColor()));
		DrawColor3("Specular",	glm::value_ptr(light->GetSpecularColor()));
		DrawFlaot("Constant",	&light->GetConstant());
		DrawFlaot("Liner",		&light->GetLinear());
		DrawFlaot("Qaudratic",	&light->GetQaudratic());
		break;
	}	
	case shade::Environment::Type::SpotLight:
	{
		auto light = static_cast<shade::SpotLight*>(environment.get());
		DrawVec3F("Position", glm::value_ptr(light->GetPosition()));
		DrawVec3F("Direction", glm::value_ptr(light->GetDirection()));
		DrawColor3("Ambient", glm::value_ptr(light->GetAmbientColor()));
		DrawColor3("Diffuse", glm::value_ptr(light->GetDiffuseColor()));
		DrawColor3("Specular", glm::value_ptr(light->GetSpecularColor()));
		DrawFlaot("Constant", &light->GetConstant());
		DrawFlaot("Liner", &light->GetLinear());
		DrawFlaot("Qaudratic", &light->GetQaudratic());
		DrawFlaot("Min", &light->GetMinAngle());
		DrawFlaot("Max", &light->GetMaxAngle());
		break;
	}
	default:
		break;
	}
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