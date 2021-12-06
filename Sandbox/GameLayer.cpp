#include "GameLayer.h"

GameLayer::GameLayer(const std::string& name) : shade::Layer(name)
{

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

void GameLayer::OnRenderBegin()
{
	shade::Render::SetClearColor(0.5, 0.5, 0, 1);
}

void GameLayer::OnRender(const shade::Shared<shade::Scene>& scene, const shade::Timer& deltaTime)
{

}

void GameLayer::OnRenderEnd()
{

}

void GameLayer::OnDelete()
{

}

void GameLayer::OnEvent(const shade::Shared<shade::Scene>& scene, shade::Event& event, const shade::Timer& deltaTime)
{

}