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
		virtual void BeginScene(const Shared<Shader>& shader, const Shared<Camera>& camera, const Shared<Environment>* enviroments = nullptr, const std::size_t& enviromentsCount = 0) override;
		virtual void EndScene(const Shared<Shader>& shader) override;
		virtual void DrawIndexed(const Drawable::DrawMode& mode, const Shared<VertexArray>& VAO, const Shared<IndexBuffer>& IBO) const override;
		virtual void DrawInstanced(const Drawable::DrawMode& mode, const Shared<VertexArray>& VAO, const Shared<IndexBuffer>& IBO, const std::uint32_t& instanceCount) override;
		virtual void DrawNotIndexed(const Drawable::DrawMode& mode, const Shared<VertexArray>& VAO, const std::uint32_t& count)  override;
		virtual void Begin(Shared<FrameBuffer> framebuffer = nullptr) override;
		virtual void End(Shared<FrameBuffer> framebuffer = nullptr) override;
		virtual void DepthTest(const bool& enable) override;
	};
}