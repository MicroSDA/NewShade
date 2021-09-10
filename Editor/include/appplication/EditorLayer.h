#pragma once
#include <shade/core/layer/ImGui/ImGuiLayer.h>

class EditorLayer : public shade::ImGuiLayer
{
public:
	EditorLayer(const std::string& name);
	virtual ~EditorLayer();
	// Inherited via Layer
	virtual void OnCreate() override;
	virtual void OnUpdate(const shade::Shared<shade::Scene>& scene, const shade::Timer& deltaTime) override;
	//virtual void OnRenderBegin() override;
	virtual void OnRender(const shade::Shared<shade::Scene>& scene, const shade::Timer& deltaTime) override;
	//virtual void OnRenderEnd() override;
	virtual void OnDelete() override;
	virtual void OnEvent(const shade::Shared<shade::Scene>& scene, shade::Event& event, const shade::Timer& deltaTime) override;
private:

	ImGuizmo::OPERATION m_GuizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
	ImGuizmo::OPERATION m_AllowedGuizmoOperation;
	ImGuizmo::OPERATION m_BasicGuizmoOperation       = ImGuizmo::OPERATION::TRANSLATE | ImGuizmo::OPERATION::ROTATE | ImGuizmo::OPERATION::SCALE;
	ImGuizmo::OPERATION m_DirectLightGuizmoOperation = ImGuizmo::OPERATION::ROTATE;
	ImGuizmo::OPERATION m_PointLightGuizmoOperation  = ImGuizmo::OPERATION::TRANSLATE | ImGuizmo::OPERATION::SCALE;
	ImGuizmo::OPERATION m_SpotLightGuizmoOperation   = ImGuizmo::OPERATION::TRANSLATE | ImGuizmo::OPERATION::ROTATE | ImGuizmo::OPERATION::SCALE;


	ImVec2 m_SceneViewPort = {0, 0};

	shade::Shared<shade::FrameBuffer>	m_FrameBuffer;

	shade::Shared<shade::Camera> m_EditorCamera;
	shade::Shared<shade::Camera> m_TestEditorCamera;
	shade::Shared<shade::Shader> m_InstancedShader;
	shade::Shared<shade::Shader> m_GridShader;
	shade::Shared<shade::Shader> m_FrustumShader;
	shade::Shared<shade::Shader> m_BloomShader;


	/*Post process*/
	shade::Shared<shade::PPBloom> m_PPBloom;
	bool						  m_isBloomEnabled = true;

	shade::Shared<shade::Grid>   m_Grid;
	shade::Shared<shade::Box>    m_Box;
	shade::Entity				 m_SelectedEntity;
	shade::Shared<shade::Material3D> m_SelectedMaterial3D;



	void MainMenu(shade::Scene* scene);
	void AssetsExplorer(shade::AssetManager::AssetsDataList& data);
	void FileExplorer(const std::string& rootPath);
	void Entities(shade::Scene* scene);
	void Inspector(shade::Entity& entity);
	void Scene(shade::Scene* scene);
	void TagComponent(shade::Entity& entity);
	void Transform3DComponent(shade::Entity& entity);
	void EnvironmentComponent(shade::Entity& entity);
	void Model3dComponent(shade::Entity& entity);
	void AssetDataExpader(shade::AssetData& data);
	void LogsExplorer();
	void Render();
	void Material(const shade::Shared<shade::Material3D>& mateial);
	// Themes
	void DarkVineTheme();


};

