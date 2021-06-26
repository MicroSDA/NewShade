#pragma once
#include "shade/core/render/buffers/IndexBuffer.h"

namespace shade
{
	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(std::uint32_t* indices, std::uint32_t count);
		virtual ~OpenGLIndexBuffer();

		virtual void Bind() const;
		virtual void Unbind() const;

		virtual uint32_t GetCount() const { return m_Count; }
	private:
		uint32_t m_RenderID;
		uint32_t m_Count;
	};

}