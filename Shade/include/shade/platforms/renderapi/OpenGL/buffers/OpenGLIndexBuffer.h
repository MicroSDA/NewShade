#pragma once
#include "shade/core/render/buffers/IndexBuffer.h"

namespace shade
{
	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(const Index* data, const std::uint32_t& count);
		OpenGLIndexBuffer(const std::uint32_t& count);
		virtual ~OpenGLIndexBuffer();

		virtual void Bind() const;
		virtual void Unbind() const;
		virtual void SetData(const Index* data, const uint32_t& count, const uint32_t& offset) override;
		virtual std::uint32_t GetCount() const override;
		virtual std::uint32_t GetSize() const override;
	private:
		uint32_t m_RenderID;

		// Inherited via IndexBuffer
	
	};

}