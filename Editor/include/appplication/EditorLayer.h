#pragma once
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
	ImGuizmo::OPERATION m_PointLightGuizmoOperation  = ImGuizmo::OPERATION::TRANSLATE;
	ImGuizmo::OPERATION m_SpotLightGuizmoOperation   = ImGuizmo::OPERATION::TRANSLATE;

	struct SceneVeiwPort
	{
		ImVec4		ViewPort = { 0, 0, 0, 0 };
		glm::vec2	MousePosition = { 0,0 };
	} m_SceneViewPort;
	
	shade::Shared<shade::FrameBuffer>	m_FrameBuffer;
	shade::Shared<shade::FrameBuffer>	m_ShadowFrameBuffer;

	shade::Shared<shade::Camera> m_EditorCamera;
	shade::Shared<shade::Camera> m_Camera;

	shade::Shared<shade::Shader> m_InstancedShader;
	shade::Shared<shade::Shader> m_ShadowShader;
	shade::Shared<shade::Shader> m_GridShader;
	shade::Shared<shade::Shader> m_FrustumShader;
	shade::Shared<shade::Shader> m_BoxShader;
	shade::Shared<shade::Shader> m_SpriteShader;
	


	/* Post process */
	shade::Shared<shade::PPBloom> m_PPBloom;
	shade::Shared<shade::Shader>  m_BloomShader;
	bool						  m_isBloomEnabled = true;
	shade::Shared<shade::PPColorCorrection> m_PPColorCorrection;
	bool						  m_isColorCorrectionEnabled = true;
	shade::Shared<shade::Shader>  m_ColorCorrectionShader;
	////////////////////////////////////

	/* Debug */
	bool m_IsShowGrid		= true;
	bool m_IsShowFrustum	= false;
	/* Relocate to renderer */
	unsigned int m_SubmitedMeshCount = 0;

	shade::Shared<shade::Grid>   m_Grid;
	shade::Shared<shade::Box>    m_Box;
	shade::Entity				 m_SelectedEntity;
	shade::Shared<shade::Material3D> m_SelectedMaterial3D;
	shade::Shared<shade::Model3D>    m_SelectedModel3D;
	shade::Shared<shade::Mesh>       m_SelectedMesh;
	/* Editor assets */
	shade::Shared<shade::Texture>	 m_IconsTexture;
	shade::Shared<shade::Texture>	 m_LogoTexture;


	void MainMenu(const shade::Shared<shade::Scene>& scene);
	void AssetsExplorer(shade::AssetManager::AssetsDataList& data);
	void FileExplorer(const std::string& rootPath);
	void Entities(shade::Scene* scene);
	void EntitiesList(const char* search, shade::Scene* scene);
	void EntitiesList(const char* search, shade::Entity& entity);
	void Inspector(shade::Entity& entity);
	void ScenePlayStop(const shade::Shared<shade::Scene>& scene);
	void Scene(const shade::Shared<shade::Scene>& scene);
	void TagComponent(shade::Entity& entity);
	void Transform3DComponent(shade::Entity& entity);
	//void EnvironmentComponent(shade::Entity& entity);

	void DirectLightComponent(shade::Entity& entity);
	void PointLightComponent(shade::Entity& entity);
	void SpotLightComponent(shade::Entity& entity);
	void CameraComponent(shade::Entity& entity);

	void Model3dComponent(shade::Entity& entity);
	void AssetDataExpader(shade::AssetData& data);
	void LogsExplorer();
	void Render();
	void ShadersLibrary();
	void Material(const shade::Shared<shade::Material3D>& mateial);
	void Model3D(const shade::Shared<shade::Model3D>& model);
	void Mesh(const shade::Shared<shade::Mesh>& mesh);
	// Themes
	void DarkVineTheme();


	/* Test section */
	shade::Shared<shade::Texture>  m_SpriteTexture;
	shade::Transform3D   		   m_SpriteTransform;
};

