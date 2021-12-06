#pragma once
#include <shade/core/layer/Layer.h>

class GameLayer : public shade::Layer
{
public:
	GameLayer(const std::string& name);
	virtual ~GameLayer();
	virtual void OnCreate() override;
	virtual void OnUpdate(const shade::Shared<shade::Scene>& scene, const shade::Timer& deltaTime) override;
	virtual void OnRenderBegin() override;
	virtual void OnRender(const shade::Shared<shade::Scene>& scene, const shade::Timer& deltaTime) override;
	virtual void OnRenderEnd() override;
	virtual void OnDelete() override;
	virtual void OnEvent(const shade::Shared<shade::Scene>& scene,  shade::Event& event, const shade::Timer& deltaTime) override;
};