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
	virtual void OnRender(const shade::Shared<shade::Scene>& scene) override;
	//virtual void OnRenderEnd() override;
	virtual void OnDelete() override;
	virtual void OnEvent(shade::Event& event) override;
private:
	shade::Shared<shade::Shader> m_Shader;
	shade::Entity m_SelectedEntity;
	void Entities(shade::Scene* scene);
	void Inspector(shade::Entity& entity);
	void TagComponent(shade::Entity& entity);
	void Transform3DComponent(shade::Entity& entity);
	void DarkVineTheme();

	shade::Shared<shade::VertexBuffer> VBO;
	shade::Shared<shade::IndexBuffer>  EBO;
	shade::Shared<shade::VertexArray>  VAO;
	std::vector<shade::Vertex3D> vertices;
	std::vector<shade::Index>    indices;
};

