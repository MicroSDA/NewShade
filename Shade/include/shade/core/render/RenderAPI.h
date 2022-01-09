#pragma once
#include "shade/config/API.h"
#include "shade/core/Types.h"
#include "shade/core/utils/Log.h"
#include "shade/core/render/buffers/VertexArray.h"
#include "shade/core/render/drawable/Drawable.h"
#include "shade/core/camera/Camera.h"
#include "shade/core/render/Shader.h"
#include "shade/core/environment/Environment.h"
#include "shade/core/render/buffers/FrameBuffer.h"
#include "shade/core/image/Texture.h"

namespace shade
{
	class SHADE_API RenderAPI
	{
	public:
		enum class API
		{
			None   = 0,
			OpenGL = 1,
			Vulkan = 2
		};
	public:
		virtual ~RenderAPI() = default;
		virtual void Init() = 0;
		virtual void ShutDown() = 0;
		virtual unsigned int GetVideoMemoryUsage() = 0;
		virtual void SetClearColor(const float& r, const float& g, const float& b, const float& a) = 0;
		virtual void Clear() = 0;
		virtual void DepthTest(const bool& enable) = 0;
		virtual void SetViewPort(std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) = 0;
		virtual void Begin() = 0;
		virtual void End() = 0;
		virtual void BeginScene(const Shared<Camera>& camera, const Shared<Environment>* env = nullptr, const std::size_t& envCount = 0) = 0;
		virtual void EndScene() = 0;
		virtual void DrawIndexed(const Drawable::DrawMode& mode, const Shared<VertexArray>& VAO, const Shared<IndexBuffer>& IBO)   const = 0;
		virtual void DrawInstanced(const Drawable::DrawMode& mode, const Shared<VertexArray>& VAO, const Shared<IndexBuffer>& IBO, const std::uint32_t& instanceCount) = 0;
		virtual void DrawNotIndexed(const Drawable::DrawMode& mode, const Shared<VertexArray>& VAO, const std::uint32_t& count) = 0;
		virtual void CullFace(const int& mode) = 0;
		virtual void Enable(const int& value) =0;
		virtual void Disable(const int& value) =0;

		static API GetAPI();
		static Unique<RenderAPI> Create();
	private:
		static API	m_sRenderAPI;
	protected:
	};
}