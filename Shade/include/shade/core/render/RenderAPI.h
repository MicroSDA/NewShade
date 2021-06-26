#pragma once
#include "shade/config/API.h"
#include "shade/core/Types.h"
#include "shade/core/utils/Log.h"
#include "shade/core/render/buffers/VertexArray.h"

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
		virtual void SetClearColor(const float& r, const float& g, const float& b, const float& a) = 0;
		virtual void Clear() = 0;
		virtual void SetViewPort(std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) = 0;
		virtual void DrawIndexed(const Shared<VertexArray>& vao) = 0;
		static API GetAPI();
		static Unique<RenderAPI> Create();
	private:
		static API	m_sRenderAPI;
	
	};
}