#pragma once
#include "shade/core/render/buffers/VertexBuffer.h"

namespace shade
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(const std::uint32_t& size, const BufferType& type);
		OpenGLVertexBuffer(const Vertex3D* data, const std::uint32_t& size, const BufferType& type);
		virtual ~OpenGLVertexBuffer();

		// Inherited via VertexBuffer
		virtual void Bind() override;
		virtual void UnBind() const override;
		virtual void SetData(const std::uint32_t& offset, const std::uint32_t& size, const Vertex3D* data) override;
		virtual void SetLayout(const Layout& layout) override;
		virtual const Layout& GetLayout() const override;
	private:
		std::uint32_t	m_RenderID;
		Layout			m_Layout;
	};
}