#include "shade_pch.h"
#include "Render.h"

shade::Unique<shade::RenderAPI> shade::Render::m_sRenderAPI = shade::RenderAPI::Create();
bool shade::Render::m_sIsInit = false;

void shade::Render::Init()
{
	if (!m_sIsInit)
	{
		m_sRenderAPI->Init();
		m_sIsInit = true;
	}
	else
		SHADE_CORE_WARNING("Render API has been already initialized!");
}

void shade::Render::SetClearColor(const float& r, const float& g, const float& b, const float& a)
{
	m_sRenderAPI->SetClearColor(r, g, b, a);
}

void shade::Render::Clear()
{
	m_sRenderAPI->Clear();
}

void shade::Render::SetViewPort(std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height)
{
	m_sRenderAPI->SetViewPort(x, y, width, height);
}

void shade::Render::DrawIndexed(const Shared<VertexArray>& vao)
{
	m_sRenderAPI->DrawIndexed(vao);
}
