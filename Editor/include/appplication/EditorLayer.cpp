#include "EditorLayer.h"
//#include <pugixml/pugixml.hpp> // Temporary
//#include <pugixml/pugiconfig.hpp> // Temporary
#include "IModel3D.h"
#include <minwindef.h> //Todo need to remove from here


EditorLayer::EditorLayer(const std::string& name) :
	shade::ImGuiLayer(name)
{
	//shade::ImGuiThemeEditor::SetColors(0x21272977, 0xFDFFFCFF, 0x621234FF, 0x13534CFF, 0x621234FF);
	shade::ImGuiThemeEditor::SetColors(0x202020FF, 0xFAFFFDFF, 0x505050FF, 0x9C1938CC, 0xFFC307B1);
	shade::ImGuiThemeEditor::SetFont("resources/font/Roboto-Medium.ttf", 14.5);
	shade::ImGuiThemeEditor::ApplyTheme();
}

EditorLayer::~EditorLayer()
{
	/* !NEED CREATE ASSET LAYOUT AND SEPARETE LAYPUT AND ASSETS*/
}

void EditorLayer::OnCreate()
{
	shade::Application::Get().GetEntities().view<shade::CameraComponent>().each([&](auto entity, auto& camera) { m_EditorCamera = camera; });

	m_FrameBuffer = shade::FrameBuffer::Create(shade::FrameBuffer::Layout(10, 10, {
		shade::FrameBuffer::Texture::Format::RGBA16F,
		shade::FrameBuffer::Texture::Format::DEPTH24STENCIL8 }));

	m_IconsTexture = shade::Texture::Create<shade::Texture>();
	m_IconsTexture->GetAssetData().Attribute("Path").set_value("resources/assets/textures/icons.dds");
	m_IconsTexture->Deserialize();
	m_IconsTexture->AssetInit();

	m_LogoTexture = shade::Texture::Create<shade::Texture>();
	m_LogoTexture->GetAssetData().Attribute("Path").set_value("resources/assets/textures/logo.dds");
	m_LogoTexture->Deserialize();
	m_LogoTexture->AssetInit();

	m_BloomShader				= shade::ShadersLibrary::Get("Bloom");
	m_ColorCorrectionShader		= shade::ShadersLibrary::Get("ColorCorrection");
	m_SpriteShader				= shade::ShadersLibrary::Get("Sprite");

	m_Grid	= shade::Grid::Create(0, 0, 0);
	m_Box	= shade::Box::Create(glm::vec3(-1.f), glm::vec3(1.f));

	m_PPBloom = shade::PPBloom::Create();
	m_PPBloom->SetInOutTargets(m_FrameBuffer, m_FrameBuffer, m_BloomShader);
	m_PPColorCorrection = shade::PPColorCorrection::Create();
	m_PPColorCorrection->SetInOutTargets(m_FrameBuffer, m_FrameBuffer, m_ColorCorrectionShader);

	m_InstancedPipeline		= shade::RenderPipeline::Create<shade::InstancedPipeline>();
	m_GridPipeline			= shade::RenderPipeline::Create<shade::GridPipeline>();
	m_CameraFrustumPipeline = shade::RenderPipeline::Create<shade::CameraFrustumPipeline>();
	m_AABBPipeline			= shade::RenderPipeline::Create<shade::AABBPipeline>();
	m_DirectLightShadowMapPipeline	= shade::RenderPipeline::Create<shade::DirectLightShadowMapPipeline>();
	m_PointLightShadowMapPipeline = shade::RenderPipeline::Create<shade::PointLightShadowMapPipeline>();
	m_SpotLightShadowMapPipeline = shade::RenderPipeline::Create<shade::SpotLightShadowMapPipeline>();
	m_SpherePipeline = shade::RenderPipeline::Create<shade::SpherePipeline>();
}

void EditorLayer::OnUpdate(const shade::Shared<shade::Scene>& scene, const shade::Timer& deltaTime)
{

	m_SubmitedPointLightSources = 0;
	m_SubmitedMeshCount			= 0;
	m_MeshesInsidePointLight    = 0;

	/* Set curret camera */
	if (scene->IsPlaying())
	{
		auto camera = scene->GetPrimaryCamera();
		if (camera)
			m_Camera = camera.GetComponent<shade::CameraComponent>();
		else
			m_Camera = m_EditorCamera;
	}
	else
		m_Camera = m_EditorCamera;
	/*------------------------------------*/

	auto frustum = m_Camera->GetFrustum();

	/* Materials always nullptr for this*/
	if (m_IsShowGrid)
		shade::Render::SubmitPipelineInstanced(m_GridPipeline, m_Grid, m_Grid->GetMaterial(), glm::mat4(1));

	scene->GetEntities().view<shade::CameraComponent>().each([&](auto& entity, shade::CameraComponent& camera)
		{
			shade::Render::SubmitPipelineInstanced(m_CameraFrustumPipeline, m_Box, nullptr, glm::inverse(camera->GetViewProjection()));
		});


	/* Light */
	scene->GetEntities().view<shade::DirectLightComponent, shade::Transform3DComponent>().each([&](auto& entity, shade::DirectLightComponent& source, shade::Transform3DComponent& transform)
		{
			shade::Render::SubmitLight(source, scene->ComputePCTransform(entity));
		});
	scene->GetEntities().view<shade::PointLightComponent, shade::Transform3DComponent>().each([&](auto& entity, shade::PointLightComponent& source, shade::Transform3DComponent& transform)
		{
			auto cpcTransform  = scene->ComputePCTransform(entity);
			glm::vec3 position(cpcTransform[3].x, cpcTransform[3].y, cpcTransform[3].z);
			if (frustum.IsInFrustum(position, source->GetDistance()))
			{
				shade::Render::SubmitLight(source, cpcTransform);
				shade::Render::SubmitPipelineInstanced(m_SpherePipeline, shade::Sphere::Create(source->GetDistance(), 100.0f, 10.0f), nullptr, cpcTransform);
				m_SubmitedPointLightSources++;
			}
		});
	scene->GetEntities().view<shade::SpotLightComponent, shade::Transform3DComponent>().each([&](auto& entity, shade::SpotLightComponent& source, shade::Transform3DComponent& transform)
		{
			shade::Render::SubmitLight(source, scene->ComputePCTransform(entity));
			shade::Render::SubmitPipelineInstanced(m_CameraFrustumPipeline, m_Box, nullptr, (transform.GetModelMatrix()));
		});



	scene->GetEntities().view<shade::Model3DComponent, shade::Transform3DComponent>().each([&](auto& entity, shade::Model3DComponent& model, shade::Transform3DComponent& transform)
		{
			for (auto& mesh : model->GetMeshes())
			{
				auto cpcTransform = scene->ComputePCTransform(entity);
				/* Shadow pass outside of frustum*/
				shade::Render::SubmitPipelineInstanced(m_DirectLightShadowMapPipeline, mesh, mesh->GetMaterial(), cpcTransform);
				shade::Render::SubmitPipelineInstanced(m_SpotLightShadowMapPipeline, mesh, mesh->GetMaterial(), cpcTransform);

				auto& points = shade::Render::GetSubmitedLight().PointLightSources;
				for (auto& light : points)
				{
					if (shade::PointLight::IsInside(light.Position, light.Distance, cpcTransform, mesh->GetMinHalfExt(), mesh->GetMaxHalfExt()))
					{
						shade::Render::SubmitPipelineInstanced(m_PointLightShadowMapPipeline, mesh, mesh->GetMaterial(), cpcTransform);
						m_MeshesInsidePointLight++;
						break;
					}
				}
			
				//shade::Render::SubmitPipelineInstanced(m_SpotLightShadowMapPipeline, mesh, mesh->GetMaterial(), cpcTransform);

				if (frustum.IsInFrustum(cpcTransform, mesh->GetMinHalfExt(), mesh->GetMaxHalfExt()))
				{

					shade::Render::SubmitPipelineInstanced(m_InstancedPipeline, mesh, mesh->GetMaterial(), cpcTransform);
					if(m_IsShowAABB)
						shade::Render::SubmitPipelineInstanced(m_AABBPipeline, shade::Box::Create(mesh->GetMinHalfExt(), mesh->GetMaxHalfExt()), nullptr, cpcTransform);
					m_SubmitedMeshCount++;
				}
			}
		});

	
}

void EditorLayer::OnRender(const shade::Shared<shade::Scene>& scene, const shade::Timer& deltaTime)
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

		if (!scene->IsPlaying())
		{
			MainMenu(scene);
			ShowWindowBar("Entities", &EditorLayer::Entities, this, scene.get());
			ShowWindowBar("Inspector", &EditorLayer::Inspector, this, m_SelectedEntity);
			ShowWindowBar("File explorer", &EditorLayer::FileExplorer, this, "");
			ShowWindowBar("Asset explorer", &EditorLayer::AssetsExplorer, this, shade::AssetManager::GetAssetsDataList());
			ShowWindowBar("Logs", &EditorLayer::LogsExplorer, this);
			ShowWindowBar("Render", &EditorLayer::Render, this);
			ShowWindowBar("Model", &EditorLayer::Model3D, this, m_SelectedModel3D);
			ShowWindowBar("Mesh", &EditorLayer::Mesh, this, m_SelectedMesh);
			ShowWindowBar("Material", &EditorLayer::Material, this, m_SelectedMaterial3D);
			ShowWindowBar("Shaders library", &EditorLayer::ShadersLibrary, this);
		}
		ShowWindowBar("Scene", &EditorLayer::Scene, this, scene);
		

	} ImGui::End(); // Begin("DockSpace")
	{
		m_FrameBuffer->Clear(shade::AttacmentClearFlag::Color | shade::AttacmentClearFlag::Depth | shade::AttacmentClearFlag::Stensil);
		shade::Render::Begin();
			shade::Render::BeginScene(m_Camera, m_FrameBuffer);
				shade::Render::ExecuteSubmitedPipeline(m_DirectLightShadowMapPipeline);
				shade::Render::ExecuteSubmitedPipeline(m_PointLightShadowMapPipeline);
				shade::Render::ExecuteSubmitedPipeline(m_SpotLightShadowMapPipeline);

				shade::Render::ExecuteSubmitedPipeline(m_InstancedPipeline);
				shade::Render::ExecuteSubmitedPipeline(m_AABBPipeline);
				shade::Render::ExecuteSubmitedPipeline(m_SpherePipeline);

				if (m_isBloomEnabled)
					shade::Render::PProcess::Process(m_PPBloom);
				if (m_isColorCorrectionEnabled)
					shade::Render::PProcess::Process(m_PPColorCorrection);
				shade::Render::ExecuteSubmitedPipeline(m_GridPipeline);

				//shade::Render::DrawSprite(m_SpriteShader, m_ShadowMapPipeline->GetResult()->GetDepthAttachment(), glm::mat4(1));
			shade::Render::EndScene();
		shade::Render::End();
		/*if (m_isBloomEnabled)
			shade::Render::PProcess::Process(m_PPBloom);
		if (m_isColorCorrectionEnabled)
			shade::Render::PProcess::Process(m_PPColorCorrection);

		shade::Render::BeginScene(m_Camera->GetRenderData());
		shade::Render::DrawSubmited(m_GridShader);
		shade::Render::DrawInstances(m_FrustumShader);
		shade::Render::DrawSubmited(m_BoxShader);

		// Sprite
		shade::Transform2D transform;
		// Fixed aspect ratio
		float scale = 1.0f;
		transform.SetScale(glm::vec2(10.f / 120.f, 10.f / 199.f));
		transform.SetPostition(-0.9f, -0.9f);

		shade::Render::DrawSprite(m_SpriteShader, m_LogoTexture, transform.GetModelMatrix(), glm::vec4{ 120, 199.0f, 574, 167 });
		
		shade::Render::EndScene();*/
		shade::Render::End();
	}
}

void EditorLayer::OnDelete()
{

}

void EditorLayer::OnEvent(const shade::Shared<shade::Scene>& scene, shade::Event& event, const shade::Timer& deltaTime)
{
	OnImGuiEvent(event);

	if (event.GetEventType() == shade::EventType::KeyPressed)
	{
		shade::KeyCode keyCode = static_cast<shade::KeyEvent*>(&event)->GetKeyCode();

		if (keyCode == shade::Key::Delete)
		{
			scene->DestroyEntities();
		}
		if (m_SelectedEntity.IsValid() && keyCode == shade::Key::F)
		{
			switch (m_GuizmoOperation)
			{
			case ImGuizmo::OPERATION::TRANSLATE:
				if (m_AllowedGuizmoOperation & ImGuizmo::OPERATION::ROTATE)
					m_GuizmoOperation = ImGuizmo::OPERATION::ROTATE; break;
			case ImGuizmo::OPERATION::ROTATE:
				if (m_AllowedGuizmoOperation & ImGuizmo::OPERATION::SCALE)
					m_GuizmoOperation = ImGuizmo::OPERATION::SCALE; break;
			case ImGuizmo::OPERATION::SCALE:
				if (m_AllowedGuizmoOperation & ImGuizmo::OPERATION::TRANSLATE)
					m_GuizmoOperation = ImGuizmo::OPERATION::TRANSLATE; break;
			}
		}
		if (keyCode == shade::Key::F1)
		{
			shade::AssetManager::HoldPrefab<shade::Model3D>("Cube", [&](auto& asset) mutable
				{
					auto entity = scene->CreateEntity("Cube");
					entity.AddComponent<shade::Model3DComponent>(shade::AssetManager::Receive<shade::Model3D>(asset));
					entity.AddComponent<shade::Transform3DComponent>();
				}, shade::Asset::Lifetime::Destroy);
		}
	}
	if (event.GetEventType() == shade::EventType::MouseButtonPressed)
	{
		if (shade::Input::IsMouseButtonPressed(shade::Mouse::ButtonLeft))
		{
			auto id = m_FrameBuffer->GetData(1, m_SceneViewPort.MousePosition.x, m_SceneViewPort.MousePosition.y).R;
			/*if (scene->GetEntity(id).IsValid())
				m_SelectedEntity =  scene->GetEntity(id);*/
		}
	}
	if (event.GetEventType() == shade::EventType::WindowResize)
	{

		auto& window = shade::Application::Get().GetWindow();
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
				if (ImGui::MenuItem("Dirrect"))
				{
					auto entity = scene->CreateEntity("Direct light");
					entity.AddComponent<shade::DirectLightComponent>(shade::CreateShared<shade::DirectLight>());
					entity.AddComponent<shade::Transform3DComponent>();
				}
				if (ImGui::MenuItem("Point"))
				{
					auto entity = scene->CreateEntity("Point light");
					entity.AddComponent<shade::PointLightComponent>(shade::CreateShared<shade::PointLight>());
					entity.AddComponent<shade::Transform3DComponent>();
				}
				if (ImGui::MenuItem("Spot"))
				{
					auto entity = scene->CreateEntity("Spot light");
					entity.AddComponent<shade::SpotLightComponent>(shade::CreateShared<shade::SpotLight>());
					entity.AddComponent<shade::Transform3DComponent>();
				}

				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Camera"))
			{
				auto entity = scene->CreateEntity("Camera");
				entity.AddComponent<shade::CameraComponent>(shade::CreateShared<shade::Camera>());
			}

			ImGui::EndPopup();
		}

		EntitiesList(search, scene);

		ImGui::ListBoxFooter();

		if (m_SelectedEntity.IsValid())
		{
			if (ImGui::BeginPopupContextItem("##ComponentPopup"))
			{

				AddChild("New entity as child", m_SelectedEntity);
				UnsetParent("Unset parent", m_SelectedEntity);

				ImGui::Separator();
				AddComponent<shade::Transform3DComponent>("Transform3D", false, m_SelectedEntity, [](shade::Entity& entity)
					{
						entity.AddComponent<shade::Transform3DComponent>();
					}, m_SelectedEntity);
				AddComponent<shade::Model3DComponent>("Model3D", true, m_SelectedEntity, [&](shade::Entity& entity)
					{
						for (auto& asset : shade::AssetManager::GetPrefabsDataList())
						{
							if (strcmp(asset.second.Attribute("Type").as_string(), "Model3D") == 0)
								if (ImGui::MenuItem(asset.first.c_str()))
								{
									shade::AssetManager::HoldPrefab<shade::Model3D>(asset.first,
										[&](auto& model) mutable
										{
											m_SelectedEntity.AddComponent<shade::Model3DComponent>(shade::AssetManager::Receive<shade::Model3D>(model));
										});
								}
						}
					}, m_SelectedEntity);

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

void EditorLayer::EntitiesList(const char* search, shade::Scene* scene)
{
	static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
	scene->GetEntities().view<std::string>().each([&]
	(auto entity_id, std::string& tag)
		{
			shade::Entity	entity = { entity_id, scene };
			std::string		_tag = tag + "##" + entity;

			if (!entity.GetParent().IsValid())
			{
				if (tag.find(search) != std::string::npos)
				{
					ImGuiTreeNodeFlags node_flags = base_flags;
					if (entity.GetChildren().size() == 0)
						node_flags |= ImGuiTreeNodeFlags_Leaf;

					if (m_SelectedEntity == entity)
						node_flags |= ImGuiTreeNodeFlags_Selected;

					bool node_open = ImGui::TreeNodeEx(_tag.c_str(), node_flags);
					if (ImGui::IsItemClicked())
						m_SelectedEntity = entity;

					if (ImGui::BeginDragDropTarget())
					{
						const auto payload = ImGui::AcceptDragDropPayload("_ENTITY_DRAG_AND_DROP");
						if (payload)
							entity.AddChild(*(shade::Entity*)payload->Data);
						ImGui::EndDragDropTarget();
					}

					if (ImGui::BeginDragDropSource())
					{
						ImGui::SetDragDropPayload("_ENTITY_DRAG_AND_DROP", &m_SelectedEntity, sizeof(shade::Entity));
						ImGui::Text("Add as child");
						ImGui::EndDragDropSource();
					}

					if (node_open)
					{
						EntitiesList(search, entity);
						ImGui::TreePop();
					}
				}
			}
		});
}

void EditorLayer::EntitiesList(const char* search, shade::Entity& entity)
{
	static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
	for (auto& child : entity.GetChildren())
	{
		std::string& tag = child.GetComponent<shade::Tag>();
		std::string	_tag = tag + "##" + child;

		if (tag.find(search) != std::string::npos)
		{
			ImGuiTreeNodeFlags node_flags = base_flags;
			if (child.GetChildren().size() == 0)
				node_flags |= ImGuiTreeNodeFlags_Leaf;

			if (m_SelectedEntity == child)
				node_flags |= ImGuiTreeNodeFlags_Selected;

			bool node_open = ImGui::TreeNodeEx(_tag.c_str(), node_flags);
			if (ImGui::IsItemClicked())
				m_SelectedEntity = child;

			if (ImGui::BeginDragDropTarget())
			{
				const auto payload = ImGui::AcceptDragDropPayload("_ENTITY_DRAG_AND_DROP");
				if (payload)
					child.AddChild(*(shade::Entity*)payload->Data);
				ImGui::EndDragDropTarget();
			}

			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("_ENTITY_DRAG_AND_DROP", &m_SelectedEntity, sizeof(shade::Entity));
				ImGui::Text("Add as child");
				ImGui::EndDragDropSource();
			}

			if (node_open)
			{
				EntitiesList(search, child);
				ImGui::TreePop();
			}
		}
	}
}

void EditorLayer::Inspector(shade::Entity& entity)
{
	static ImVec4 editIcon = ImVec4{ 128, 128, 897, 750 };

	if (entity.IsValid())
	{
		TagComponent(entity);
		DrawComponent<shade::Transform3DComponent>("Transform", entity, &EditorLayer::Transform3DComponent, [&](auto isTreeOpen)->bool
			{
				return EditComponentButton<shade::Transform3DComponent>(entity, m_IconsTexture, editIcon, isTreeOpen);
			}, this, entity);
		DrawComponent<shade::Model3DComponent>("Model", entity, &EditorLayer::Model3dComponent, [&](auto isTreeOpen)->bool
			{
				return EditComponentButton<shade::Model3DComponent>(entity, m_IconsTexture, editIcon, isTreeOpen);
			}, this, entity);
		DrawComponent<shade::DirectLightComponent>("Direct light", entity, &EditorLayer::DirectLightComponent, [&](auto isTreeOpen)->bool
			{
				return EditComponentButton<shade::DirectLightComponent>(entity, m_IconsTexture, editIcon, isTreeOpen);
			}, this, entity);
		DrawComponent<shade::PointLightComponent>("Point light", entity, &EditorLayer::PointLightComponent, [&](auto isTreeOpen)->bool
			{
				return EditComponentButton<shade::PointLightComponent>(entity, m_IconsTexture, editIcon, isTreeOpen);
			}, this, entity);
		DrawComponent<shade::SpotLightComponent>("Spot light", entity, &EditorLayer::SpotLightComponent, [&](auto isTreeOpen)->bool
			{
				return EditComponentButton<shade::SpotLightComponent>(entity, m_IconsTexture, editIcon, isTreeOpen);
			}, this, entity);
		DrawComponent<shade::CameraComponent>("Camera", entity, &EditorLayer::CameraComponent, [&](auto isTreeOpen)->bool
			{
				return EditComponentButton<shade::CameraComponent>(entity, m_IconsTexture, editIcon, isTreeOpen);
			}, this, entity);
	}
}

void EditorLayer::ScenePlayStop(const shade::Shared<shade::Scene>& scene)
{
	ImVec4 buttonProps = { 20,20, 128, 128 };
	if (m_GuizmoOperation == 0)
		DrawButtonImage("##Select", m_IconsTexture, { buttonProps.x, buttonProps.y }, { buttonProps.z, buttonProps.w }, { 8, 750 }, -1, ImGui::GetStyle().Colors[ImGuiCol_PlotLines]);
	else
	{
		if (DrawButtonImage("##Select", m_IconsTexture, { buttonProps.x, buttonProps.y }, { buttonProps.z, buttonProps.w }, { 8, 750 }, -1, ImGui::GetStyle().Colors[ImGuiCol_Text]))
			m_GuizmoOperation = ImGuizmo::OPERATION(0);
	}

	ImGui::SameLine();
	if (m_GuizmoOperation == ImGuizmo::TRANSLATE)
		DrawButtonImage("##TRANSLATE", m_IconsTexture, { buttonProps.x, buttonProps.y }, { buttonProps.z, buttonProps.w }, { 159, 158 }, -1, ImGui::GetStyle().Colors[ImGuiCol_PlotLines]);
	else
	{
		if (DrawButtonImage("##TRANSLATE", m_IconsTexture, { buttonProps.x, buttonProps.y }, { buttonProps.z, buttonProps.w }, { 159, 158 }, -1, ImGui::GetStyle().Colors[ImGuiCol_Text]))
			m_GuizmoOperation = ImGuizmo::TRANSLATE;
	}

	ImGui::SameLine();
	if (m_GuizmoOperation == ImGuizmo::ROTATE)
		DrawButtonImage("##ROTATE", m_IconsTexture, { buttonProps.x, buttonProps.y }, { buttonProps.z, buttonProps.w }, { 454, 898 }, -1, ImGui::GetStyle().Colors[ImGuiCol_PlotLines]);
	else
	{
		if (DrawButtonImage("##ROTATE", m_IconsTexture, { buttonProps.x, buttonProps.y }, { buttonProps.z, buttonProps.w }, { 454, 898 }, -1, ImGui::GetStyle().Colors[ImGuiCol_Text]))
			m_GuizmoOperation = ImGuizmo::ROTATE;
	}

	ImGui::SameLine();
	if (m_GuizmoOperation == ImGuizmo::SCALE)
		DrawButtonImage("##SCALE", m_IconsTexture, { buttonProps.x, buttonProps.y }, { buttonProps.z, buttonProps.w }, { 10, 10 }, -1, ImGui::GetStyle().Colors[ImGuiCol_PlotLines]);
	else
	{
		if (DrawButtonImage("##SCALE", m_IconsTexture, { buttonProps.x, buttonProps.y }, { buttonProps.z, buttonProps.w }, { 10, 10 }, -1, ImGui::GetStyle().Colors[ImGuiCol_Text]))
			m_GuizmoOperation = ImGuizmo::SCALE;
	}

	ImGui::SameLine((ImGui::GetContentRegionAvailWidth() / 2) - (25 / 2));
	if (!scene->IsPlaying())
	{
		if (DrawButtonImage("##Play", m_IconsTexture, { buttonProps.x, buttonProps.y }, { buttonProps.z, buttonProps.w }, { 900, 898 }, -1, ImVec4(0.2, 0.7, 0.2, 1.0)))
			scene->SetPlaying(true);
	}
	else
	{
		if (DrawButtonImage("##Stop", m_IconsTexture, { buttonProps.x, buttonProps.y }, { buttonProps.z, buttonProps.w }, { 602, 898 }, -1, ImVec4(0.7, 0.2, 0.2, 1.0)))
			scene->SetPlaying(false);
	}

	ImGuiIO& io = ImGui::GetIO();
	ImGui::SameLine(ImGui::GetContentRegionAvailWidth() - ImGui::CalcItemWidth() - 15);
	ImGui::Text("Application average %.1f ms/frame (%.0f FPS)", 1000.0f / io.Framerate, io.Framerate);
	ImGui::Text("Submited meshes [%d]", m_SubmitedMeshCount); 
	ImGui::SameLine(); ImGui::Text("Point lights [%d]", m_SubmitedPointLightSources);
	ImGui::SameLine(); ImGui::Text("Meshes inside point light [%d]", m_MeshesInsidePointLight);
}

void EditorLayer::Scene(const shade::Shared<shade::Scene>& scene)
{
	//SHADE_CORE_TRACE("{0}  {1}", ImGui::GetWindow();

	static ImVec4 focusColor = { 0, 0, 0, 1 };

	if (ImGui::IsWindowFocused())
	{
		focusColor = { 0.995f, 0.857f, 0.420f, 1.000f };
		if (m_Camera == m_EditorCamera)
			m_EditorCamera->SetAsPrimary(true);
		else
			m_EditorCamera->SetAsPrimary(false);
	}
	else
	{
		focusColor = { 0, 0, 0, 1 };
		m_EditorCamera->SetAsPrimary(false);
	}

	

	if (m_SceneViewPort.ViewPort.z != ImGui::GetContentRegionAvail().x || m_SceneViewPort.ViewPort.w != ImGui::GetContentRegionAvail().y)
	{
		m_SceneViewPort.ViewPort.z = ImGui::GetContentRegionAvail().x;
		m_SceneViewPort.ViewPort.w = ImGui::GetContentRegionAvail().y;

		m_FrameBuffer->Resize(m_SceneViewPort.ViewPort.z, m_SceneViewPort.ViewPort.w);
		m_Camera->Resize((float)m_SceneViewPort.ViewPort.z / (float)m_SceneViewPort.ViewPort.w);
	}
	
	m_SceneViewPort.ViewPort.x = ImGui::GetWindowPos().x + ImGui::GetCursorPos().x; // With tab size
	m_SceneViewPort.ViewPort.y = ImGui::GetWindowPos().y + ImGui::GetCursorPos().y; // With tab size

	m_SceneViewPort.MousePosition.x = (unsigned int)(ImGui::GetMousePos().x - m_SceneViewPort.ViewPort.x);
	/*Flip Y coord*/
	m_SceneViewPort.MousePosition.y = (unsigned int)(m_SceneViewPort.ViewPort.w - (ImGui::GetMousePos().y - m_SceneViewPort.ViewPort.y));

	ImGui::Image(reinterpret_cast<void*>(m_FrameBuffer->GetAttachment(0)), { m_SceneViewPort.ViewPort.z, m_SceneViewPort.ViewPort.w }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 }, ImVec4{ 1, 1, 1, 1 }, focusColor);

	
	//SHADE_CORE_TRACE("Selected x :{0}, y :{1}", m_SceneViewPort.MousePosition.x, m_SceneViewPort.MousePosition.y);

	ImGui::SetNextWindowSize(ImVec2{ ImGui::GetWindowSize().x - 20.0f,0 }, ImGuiCond_Always);
	ShowWindowBarOverlay("Overlay", ImGui::GetWindowViewport(), [&]()
		{
			ScenePlayStop(scene);
		});
	
	

	if (m_SelectedEntity.IsValid())
	{
		if (m_SelectedEntity.HasComponent<shade::Transform3DComponent>())
		{
			m_AllowedGuizmoOperation = m_BasicGuizmoOperation;
			auto& transform = m_SelectedEntity.GetComponent<shade::Transform3DComponent>();
			auto pcTransform = scene->ComputePCTransform(m_SelectedEntity);

			if (DrawImGuizmo(pcTransform, m_Camera, m_GuizmoOperation, ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowSize().x, ImGui::GetWindowSize().y))
			{
				if (m_SelectedEntity.HasParent())
				{
					auto parentTransform = scene->ComputePCTransform(m_SelectedEntity.GetParent());
					pcTransform = glm::inverse(parentTransform) * pcTransform;
				}


				/*glm::vec3 translation, rotation, scale;
				shade::math::DecomposeMatrix(pcTransform, translation, rotation, scale);

				glm::vec3 deltaRotation = rotation - transform.GetRotation();
				transform.SetPostition(translation);
				transform.GetRotation() += deltaRotation;*/
				//transform.SetScale(scale);

				transform.SetTransform(pcTransform);
			}
		}
		else if (m_SelectedEntity.HasComponent<shade::CameraComponent>())
		{
			auto& camera = m_SelectedEntity.GetComponent<shade::CameraComponent>();
			glm::mat4 transform = camera->GetView();
			// TODO Camera parent trnasform, в сабмите и он рендер
			/*if (m_SelectedEntity.HasParent())
			{
				auto parentTransform = scene->ComputePCTransform(m_SelectedEntity.GetParent());
				transform = (parentTransform) * transform;
			}
			else*/
			{
				transform = glm::inverse(transform);
			}


			if (DrawImGuizmo(transform, m_Camera, m_GuizmoOperation, ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowSize().x, ImGui::GetWindowSize().y))
			{
				camera->SetVeiw(transform);
			}
		}
	}
}

void EditorLayer::TagComponent(shade::Entity& entity)
{
	if (entity.IsValid())
	{
		static char buffer[256]; memset(buffer, 0, sizeof(buffer));
		auto& tag = entity.GetComponent<shade::Tag>();
		std::strncpy(buffer, tag.c_str(), sizeof(buffer));

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Name");
		ImGui::SameLine(50);
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
		if (ImGui::InputTextWithHint("##Tag", tag.c_str(), buffer, sizeof(buffer)))
			tag = std::string(buffer);
	}
}

void EditorLayer::FileExplorer(const std::string& rootPath)
{
	ImGui::Text("FileExplorer");

}

void EditorLayer::MainMenu(const shade::Shared<shade::Scene>& scene)
{

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::BeginMenu("Scene"))
			{
				if (ImGui::MenuItem("Open scene"))
				{
					auto path = shade::FileDialog::OpenFile("Shade scene(*.scene) \0*.scene\0");
					if (!path.empty())
					{
						/*shade::AssetManager::Hold<shade::Scene>("Scene", shade::Asset::State::RemoveIfPosible, [](auto& asset)
							{
								shade::Application::Get().SetCurrentScene(shade::AssetManager::Receive<shade::Scene>(asset));
							});*/

						auto scene = shade::Scene::Create();
						scene->GetAssetData().Attribute("Path").set_value(path.string().c_str());
						scene->Deserialize();
						shade::Application::Get().SetCurrentScene(scene);
						m_SelectedEntity = shade::Entity();
					}
				}

				if (ImGui::MenuItem("Save scene"))
				{
					auto path = shade::FileDialog::SaveFile("Shade scene(*.scene) \0*.scene\0");
					if (!path.empty())
					{
						scene->GetAssetData().Attribute("Id").set_value(path.stem().string().c_str());
						scene->GetAssetData().Attribute("Path").set_value(path.string().c_str());
						scene->Serialize();
					}

				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Import"))
			{
				if (ImGui::MenuItem("Model"))
				{
					auto path = shade::FileDialog::OpenFile("Supported formats(*.obj, *.fbx, *.dae) \0*.obj;*.fbx;*.dae\0");
					if (!path.empty())
					{
						auto model = IModel3D::Import(path.string());
						if (model)
						{
							auto entity = scene->CreateEntity(path.filename().string());
							entity.AddComponent<shade::Transform3DComponent>();
							entity.AddComponent<shade::Model3DComponent>(model);
						}
					}
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}





		ImGui::EndMenuBar();
	}


}

void EditorLayer::AssetsExplorer(shade::AssetManager::AssetsDataList& data)
{
	for (auto& asset : data)
	{
		if (ImGui::BeginMenu(asset.second.Attribute("Type").as_string()))
		{
			AssetDataExpader(asset.second);

			ImGui::EndMenu();
		}

	}
}
/* Удвлить старый функционал с лайта*/
void EditorLayer::Transform3DComponent(shade::Entity& entity)
{
	auto& transform = entity.GetComponent<shade::Transform3DComponent>();
	DrawVec3F("Position", glm::value_ptr(transform.GetPosition()));
	DrawVec3F("Roatation", glm::value_ptr(transform.GetRotation()));
	DrawVec3F("Scale", glm::value_ptr(transform.GetScale()), 1.0f);
}

void EditorLayer::DirectLightComponent(shade::Entity& entity)
{
	auto& light = entity.GetComponent<shade::DirectLightComponent>();
	DrawFlaot("Intensity", &light->GetIntensity());
	DrawColor3("Diffuse", glm::value_ptr(light->GetDiffuseColor()));
	DrawColor3("Specular", glm::value_ptr(light->GetSpecularColor()));
}

void EditorLayer::PointLightComponent(shade::Entity& entity)
{
	auto& light = entity.GetComponent<shade::PointLightComponent>();
	DrawFlaot("Intensity", &light->GetIntensity());
	DrawColor3("Diffuse", glm::value_ptr(light->GetDiffuseColor()));
	DrawColor3("Specular", glm::value_ptr(light->GetSpecularColor()));
	DrawFlaot("Distance", &light->GetDistance());
	DrawFlaot("Falloff", &light->GetFalloff());
}

void EditorLayer::SpotLightComponent(shade::Entity& entity)
{
	auto& light = entity.GetComponent<shade::SpotLightComponent>();
	DrawFlaot("Intensity", &light->GetIntensity());
	DrawColor3("Diffuse", glm::value_ptr(light->GetDiffuseColor()));
	DrawColor3("Specular", glm::value_ptr(light->GetSpecularColor()));
	DrawFlaot("Distance", &light->GetDistance());
	DrawFlaot("Falloff", &light->GetFalloff());
	DrawFlaot("Min", &light->GetMinAngle());
	DrawFlaot("Max", &light->GetMaxAngle());
}

void EditorLayer::CameraComponent(shade::Entity& entity)
{
	auto& camera = entity.GetComponent<shade::CameraComponent>();

	DrawVec3F("Position", glm::value_ptr(camera->GetPosition()));
	DrawVec3F("Diretction", glm::value_ptr(camera->GetForwardDirrection()));

	float fov = camera->GetFov();
	float znear = camera->GetNear();
	float zfar = camera->GetFar();

	if (DrawFlaot("Fov", &fov))
		camera->SetFov(fov);
	if (DrawFlaot("Near", &znear))
		camera->SetNear(znear);
	if (DrawFlaot("Far", &zfar))
		camera->SetFar(zfar);

	bool isPrimary = camera->IsPrimary();
	if (ImGui::Checkbox("Primary", &isPrimary))
		camera->SetAsPrimary(isPrimary);

}

void EditorLayer::AssetDataExpader(shade::AssetData& data)
{
	if (data.Dependencies().size())
	{
		if (ImGui::BeginMenu(data.Attribute("Id").as_string()))
		{
			for (auto& asset : data.Dependencies())
			{
				AssetDataExpader(asset);
			}

			ImGui::EndMenu();
		}
	}
	else
	{
		if (ImGui::MenuItem(data.Attribute("Id").as_string()))
		{
			// Preveiw i guess
			if (strcmp(data.Attribute("Type").as_string(), "Texture") == 0)
				DrawImage(1, 100, 100, false);
		}
	}
}

void EditorLayer::LogsExplorer()
{
	static unsigned int oldSize = 0;
	unsigned int		currentsize = 0;

	auto& logs = shade::Log::GetClientStream();

	std::stringstream  stream;
	std::string	_info;
	std::string	_trace;
	std::string	_warning;
	std::string	_error;

	stream << logs.str();

	ImGui::BeginGroup();
	ImGui::BeginChild("Logs", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us


	for (std::string line; std::getline(stream, line); ) {
		if (line.find("[info]") != std::string::npos)
		{

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2, 0.5, 0.2, 1.0));
			ImGui::TextWrapped(line.c_str());
			ImGui::PopStyleColor();
		}
		else if (line.find("[trace]") != std::string::npos)
		{

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3, 0.3, 0.8, 1.0));
			ImGui::TextWrapped(line.c_str());
			ImGui::PopStyleColor();
		}
		else if (line.find("[warning]") != std::string::npos)
		{

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5, 0.5, 0.2, 1.0));
			ImGui::TextWrapped(line.c_str());
			ImGui::PopStyleColor();
		}
		else if (line.find("[error]") != std::string::npos)
		{

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5, 0.2, 0.2, 1.0));
			ImGui::TextWrapped(line.c_str());
			ImGui::PopStyleColor();
		}

		if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
			ImGui::SetScrollHereY(1.0f);
	}

	ImGui::EndChild();
	ImGui::Separator();
	if (ImGui::Button("Clear")) { shade::Log::GetClientStream().str(""); shade::Log::GetClientStream().clear(); }
	ImGui::SameLine();
	ImGui::EndGroup();


}

void EditorLayer::Model3dComponent(shade::Entity& entity)
{
	auto& model = entity.GetComponent<shade::Model3DComponent>();

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, 80.f);
	ImGui::Text("Asset");
	ImGui::NextColumn();

	if (ImGui::BeginMenu(model->GetAssetData().Attribute("Id").as_string()))
	{
		for (auto& asset : shade::AssetManager::GetPrefabsDataList())
		{
			if (strcmp(asset.second.Attribute("Type").as_string(), "Model3D") == 0)
			{
				if (ImGui::MenuItem(asset.second.Attribute("Id").as_string()))
				{
					shade::AssetManager::HoldPrefab<shade::Model3D>(asset.second.Attribute("Id").as_string(),
						[&](auto& asset) mutable
						{
							model = shade::AssetManager::Receive<shade::Model3D>(asset);
						});
				}
			}

		}
		ImGui::EndMenu();
	}
	ImGui::Columns(1);


	ImGui::AlignTextToFramePadding();
	if (ImGui::TreeNodeEx("Meshes", ImGuiTreeNodeFlags_Framed))
	{
		for (auto& mesh : model->GetMeshes())
		{
			auto meshId = mesh->GetAssetData().Attribute("Id").as_string();
			if (DrawButtonCol("Asset", meshId))
			{
				m_SelectedMaterial3D = mesh->GetMaterial();
			}
			/*if (ImGui::TreeNodeEx(std::string("Material: Render##" + std::string(meshId)).c_str(), ImGuiTreeNodeFlags_Framed))
			{
				m_SelectedMaterial3D = mesh->GetMaterial();
				//Material(m_SelectedMaterial3D);
				ImGui::TreePop();
			}*/
		}
		ImGui::TreePop();
	}
}

void EditorLayer::Render()
{

	static float multiplier = 1.0f;
	if (DrawFlaot("Bias", &multiplier))
	{
		m_PointLightShadowMapPipeline->SetMultiplier(multiplier);
	}

	if (ImGui::TreeNodeEx("Bloom", ImGuiTreeNodeFlags_Framed))
	{

		ImGui::Checkbox("Enable", &m_isBloomEnabled);
		DrawFlaot("Threshold", &m_PPBloom->GetThreshold(), 1.f, 0.f, FLT_MAX, 80.f);
		DrawFlaot("Knee", &m_PPBloom->GetKnee(), 1.f, 0.f, FLT_MAX, 80.f);
		DrawFlaot("Exposure", &m_PPBloom->GetExposure(), 1.f, 1.f, FLT_MAX, 80.f);
		DrawInt("Samples", (int*)&m_PPBloom->GetSamplesCount(), 5, 1, 10, 80.f);
		DrawCurve("Curve", glm::value_ptr(m_PPBloom->GetCurve()), 3, ImVec2{ ImGui::GetContentRegionAvail().x, 70 });
		ImGui::TreePop();
	}
	if (ImGui::TreeNodeEx("Color correction", ImGuiTreeNodeFlags_Framed))
	{

		ImGui::Checkbox("Enable", &m_isColorCorrectionEnabled);
		DrawFlaot("Gamma", &m_PPColorCorrection->GetGamma(), 1.f, 0.f, FLT_MAX, 80.f);
		DrawFlaot("Exposure", &m_PPColorCorrection->GetExposure(), 1.f, 0.f, FLT_MAX, 80.f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNodeEx("Shadows ", ImGuiTreeNodeFlags_Framed))
	{
		static bool dShadowsCast = true;
		static bool sShadowsCast = true;
		static bool pShadowsCast = true;

		ImGui::Checkbox("Direct shadows", &dShadowsCast);
		ImGui::Checkbox("Spot shadows",   &sShadowsCast);
		ImGui::Checkbox("Point shadows",  &pShadowsCast);

		ImGui::TreePop();
	}

	ImGui::Separator();
	ImGui::Checkbox("Show grid",    &m_IsShowGrid);
	ImGui::Checkbox("Show frustum", &m_IsShowFrustum);
	ImGui::Checkbox("Show AABB",    &m_IsShowAABB);
	ImGui::Text("Submited meshes [%d]", m_SubmitedMeshCount);
	ImGui::Text("Video memory in usage: %d(mbs)", shade::Render::GetVideoMemoryUsage());
}

void EditorLayer::ShadersLibrary()
{

	for (auto& [key, val] : shade::ShadersLibrary::GetLibrary())
	{
		// TODO not sure if button has coorect id in stack!
		if (DrawButtonCol(key.c_str(), "Recompile"))
			val->Recompile();
	}
}

void EditorLayer::Material(const shade::Shared<shade::Material3D>& material)
{
	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, 80.f);
	ImGui::Text("Asset");
	ImGui::NextColumn();

	if (ImGui::BeginMenu((material) ? material->GetAssetData().Attribute("Id").as_string() : "None"))
	{
		if (ImGui::MenuItem("None"))
			m_SelectedMaterial3D = nullptr;

		for (auto& asset : shade::AssetManager::GetAssetsDataList())
		{
			if (strcmp(asset.second.Attribute("Type").as_string(), "Material") == 0)
			{
				if (ImGui::MenuItem(asset.second.Attribute("Id").as_string()))
				{
					shade::AssetManager::HoldPrefab<shade::Material3D>(asset.second.Attribute("Id").as_string(),
						[&](auto& asset) mutable
						{
							m_SelectedMaterial3D = shade::AssetManager::Receive<shade::Material3D>(asset);
						});
				}
			}

		}

		ImGui::EndMenu();
	}

	ImGui::Columns(1);
	/*
	for (auto& asset : shade::AssetManager::GetAssetsDataList())
	{
		if (strcmp(asset.second.Attribute("Type").as_string(), "Material3D") == 0)
		{

		}
	}*/

	if (material)
	{
		DrawColor3("Albedo", glm::value_ptr(material->ColorDiffuse));
		DrawColor3("Specular", glm::value_ptr(material->ColorSpecular));
		DrawColor3("Ambinet", glm::value_ptr(material->ColorAmbient));
		DrawColor3("Transparent", glm::value_ptr(material->ColorTransparent));
		ImGui::Separator();
		ImGui::Text("Shading model %d", material->Shading);
		DrawFlaot("Shininess", &material->Shininess);
		DrawFlaot("Shininess power", &material->ShininessStrength);
		DrawFlaot("Opacity", &material->Opacity);
		DrawFlaot("Refracti", &material->Refracti);
		DrawFlaot("Emissive", &material->Emmisive);
		ImGui::Checkbox("Use wireframe", &material->WireFrame);
		ImGui::Separator();

		if (ImGui::TreeNodeEx("Map: Albedo", ImGuiTreeNodeFlags_Framed))
		{
			if (material->TextureDiffuse)
				DrawImage(material->TextureDiffuse->GetRenderID(), 100, 100, true);
			ImGui::TreePop();
		}
		if (ImGui::TreeNodeEx("Map: Specular", ImGuiTreeNodeFlags_Framed))
		{
			if (material->TextureDiffuse)
				DrawImage(material->TextureSpecular->GetRenderID(), 100, 100, true);
			ImGui::TreePop();
		}
		if (ImGui::TreeNodeEx("Map: Normal", ImGuiTreeNodeFlags_Framed))
		{
			if (material->TextureDiffuse)
				DrawImage(material->TextureNormals->GetRenderID(), 100, 100, true);
			ImGui::TreePop();
		}
		
		ImGui::Separator();
		if (DrawButtonImage("##Save", m_IconsTexture, { 20, 20 }, { 128, 128 }, { 306, 898 }, -1))
		{
			material->Serialize();
		}
	}
}

void EditorLayer::Model3D(const shade::Shared<shade::Model3D>& model)
{
	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, 80.f);
	ImGui::Text("Asset");
	ImGui::NextColumn();

	if (ImGui::BeginMenu((model) ? model->GetAssetData().Attribute("Id").as_string() : "None"))
	{
		if (ImGui::MenuItem("None"))
			m_SelectedModel3D = nullptr;

		for (auto& asset : shade::AssetManager::GetPrefabsDataList())
		{
			if (strcmp(asset.second.Attribute("Type").as_string(), "Model3D") == 0)
			{
				if (ImGui::MenuItem(asset.second.Attribute("Id").as_string()))
				{
					shade::AssetManager::HoldPrefab<shade::Model3D>(asset.second.Attribute("Id").as_string(),
						[&](auto& asset) mutable
						{
							m_SelectedModel3D = shade::AssetManager::Receive<shade::Model3D>(asset);
						});
				}
			}

		}

		ImGui::EndMenu();
	}

	ImGui::Columns(1);
	if (model)
	{
		ImGui::AlignTextToFramePadding();
		if (ImGui::TreeNodeEx("Meshes", ImGuiTreeNodeFlags_Framed))
		{
			for (auto& mesh : model->GetMeshes())
			{
				auto meshId = mesh->GetAssetData().Attribute("Id").as_string();
				if (DrawButtonCol("Asset", meshId))
				{
					m_SelectedMesh = mesh;
				}
				/*if (ImGui::TreeNodeEx(std::string("Material: Render##" + std::string(meshId)).c_str(), ImGuiTreeNodeFlags_Framed))
				{
					m_SelectedMaterial3D = mesh->GetMaterial();
					//Material(m_SelectedMaterial3D);
					ImGui::TreePop();
				}*/
			}
			ImGui::TreePop();
		}
	}
}

void EditorLayer::Mesh(const shade::Shared<shade::Mesh>& mesh)
{
	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, 80.f);
	ImGui::Text("Asset");
	ImGui::NextColumn();

	if (ImGui::BeginMenu((mesh) ? mesh->GetAssetData().Attribute("Id").as_string() : "None"))
	{
		if (ImGui::MenuItem("None"))
			m_SelectedMesh = nullptr;

		for (auto& asset : shade::AssetManager::GetAssetsDataList())
		{
			if (strcmp(asset.second.Attribute("Type").as_string(), "Mesh") == 0)
			{
				if (ImGui::MenuItem(asset.second.Attribute("Id").as_string()))
				{
					shade::AssetManager::HoldPrefab<shade::Mesh>(asset.second.Attribute("Id").as_string(),
						[&](auto& asset) mutable
						{
							m_SelectedMesh = shade::AssetManager::Receive<shade::Mesh>(asset);
						});
				}
			}

		}

		ImGui::EndMenu();
	}

	ImGui::Columns(1);
	if (mesh)
	{
		if (ImGui::TreeNodeEx("Material", ImGuiTreeNodeFlags_Framed))
		{
			auto materialId = mesh->GetMaterial()->GetAssetData().Attribute("Id").as_string();
			if (DrawButtonCol("Asset", materialId))
			{
				m_SelectedMaterial3D = mesh->GetMaterial();
			}
			ImGui::TreePop();
		}

	}

	if (mesh)
	{
		ImGui::Separator();

		if (DrawButtonImage("##AABB", m_IconsTexture, { 20, 20 }, { 128, 128 }, { 10, 10 }, -1))
			mesh->GenerateHalfExt();
		ImGui::SameLine();
		if (DrawButtonImage("##Save", m_IconsTexture, { 20, 20 }, { 128, 128 }, { 306, 898 }, -1))
			mesh->Serialize();
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
#define TEXT_IMGUI(v) ImVec4(0.860f, 0.930f, 0.890f, v)

	auto& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Text] = TEXT_IMGUI(0.78f);
	style.Colors[ImGuiCol_TextDisabled] = TEXT_IMGUI(0.28f);
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
	style.Colors[ImGuiCol_PlotLines] = TEXT_IMGUI(0.63f);
	style.Colors[ImGuiCol_PlotLinesHovered] = MED(1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = TEXT_IMGUI(0.63f);
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