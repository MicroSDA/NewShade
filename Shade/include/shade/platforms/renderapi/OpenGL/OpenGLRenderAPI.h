#pragma once
#include "shade/core/render/RenderAPI.h"

namespace shade
{
	class OpenGLRenderAPI : public RenderAPI
	{
	public:
		virtual void Init() override;
		virtual void ShutDown();
		virtual unsigned int GetVideoMemoryUsage() override;
		virtual void SetClearColor(const float& r, const float& g, const float& b, const float& a) override;
		virtual void Clear() override;
		virtual void SetViewPort(std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) override;
		virtual void BeginScene(const Shared<Camera>& camera, const Shared<Environment>* env = nullptr, const std::size_t& envCount = 0) override;
		virtual void EndScene() override;
		virtual void DrawIndexed(const Drawable::DrawMode& mode, const Shared<VertexArray>& VAO, const Shared<IndexBuffer>& IBO) const override;
		virtual void DrawInstanced(const Drawable::DrawMode& mode, const Shared<VertexArray>& VAO, const Shared<IndexBuffer>& IBO, const std::uint32_t& instanceCount) override;
		virtual void DrawNotIndexed(const Drawable::DrawMode& mode, const Shared<VertexArray>& VAO, const std::uint32_t& count)  override;
		virtual void Begin() override;
		virtual void End() override;
		virtual void DepthTest(const bool& enable) override;
		virtual void CullFace(const int& mode) override;
		//TODO Has to be check if this is needed !!!!!!!!!!!!
		// Post Process
		virtual void Bloom(const Shared<FrameBuffer>& inputFrameBuffer, const Shared<FrameBuffer>& outFrameBuffer, const Shared<Shader>& bloomShader) override;
	private:
		Shared<FrameBuffer> m_BloomFrameBuffer;
	};
}
