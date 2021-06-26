#pragma once
#include "shade/core/render/RenderAPI.h"

namespace shade
{
	class OpenGLRenderAPI : public RenderAPI
	{
	public:
		virtual void Init() override;
		virtual void SetClearColor(const float& r, const float& g, const float& b, const float& a) override;
		virtual void Clear() override;
		virtual void SetViewPort(std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) override;
		virtual void DrawIndexed(const Shared<VertexArray>& vao);
	};
}