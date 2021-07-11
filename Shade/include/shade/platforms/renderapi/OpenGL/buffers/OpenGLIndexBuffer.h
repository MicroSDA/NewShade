#pragma once
#include "shade/core/render/buffers/IndexBuffer.h"

namespace shade
{
	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(const Index* indices, const std::uint32_t& count);
		OpenGLIndexBuffer(const std::uint32_t& count);
		virtual ~OpenGLIndexBuffer();

		virtual void Bind() const;
		virtual void Unbind() const;
		virtual void SetData(const Index* indices, const uint32_t& count, const uint32_t& offset) override;
		virtual uint32_t GetCount() const { return m_Count; }
	private:
		uint32_t m_RenderID;
		uint32_t m_Count;

	};

}