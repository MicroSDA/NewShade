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
private:
	shade::Entity m_SelectedEntity;
	void Entities(shade::Scene* scene);
	void Inspector(shade::Entity& entity);
	void TagComponent(shade::Entity& entity);
	void Transform3DComponent(shade::Entity& entity);
};

