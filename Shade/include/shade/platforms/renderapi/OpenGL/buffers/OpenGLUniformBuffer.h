#pragma once
#include "shade/core/render/buffers/UniformBuffer.h"

namespace shade
{
	class OpenGLUniformBuffer :public UniformBuffer
	{
	public:
		OpenGLUniformBuffer(const std::uint32_t& size, const std::uint32_t& binding);
		virtual ~OpenGLUniformBuffer();

		virtual void SetData(const void* data, const std::uint32_t& size, const std::uint32_t& offset = 0) override;
	private:
		std::uint32_t m_RenderID = 0;

		// Inherited via UniformBuffer
		virtual std::uint32_t GetSize() override;
	};
}
