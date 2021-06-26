#pragma once
#include "shade/core/render/RenderAPI.h"
#include "shade/core/render/buffers/VertexArray.h"
#include "shade/core/render/Shader.h"

namespace shade
{
	class SHADE_API Render
	{
	public:
		static void Init();
		static void SetClearColor(const float& r, const float& g, const float& b,const float& a);
		static void Clear();
		static void SetViewPort(std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height);
		static void DrawIndexed(const Shared<VertexArray>& vao);
	private:
		static Unique<RenderAPI>	m_sRenderAPI;
		static bool					m_sIsInit;
	};
}