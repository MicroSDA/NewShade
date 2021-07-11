#pragma once
#include "shade/core/render/buffers/VertexBuffer.h"

namespace shade
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(const std::uint32_t& size, const BufferType& type = BufferType::Static);
		OpenGLVertexBuffer(const void* data, const std::uint32_t& size, const BufferType& type = BufferType::Static);
		virtual ~OpenGLVertexBuffer();

		// Inherited via VertexBuffer
		virtual void Bind() const override;
		virtual void UnBind() const override;
		virtual void SetData(const void* data, const uint32_t& size, const uint32_t& offset) override;
		virtual void SetLayout(const Layout& layout) override;
		virtual const Layout& GetLayout() const override;
		virtual const std::uint32_t& GetRenderID() const override;
		virtual void Copy(const Shared<VertexBuffer>& other, const std::uint32_t& size, const std::uint32_t& readOffset, const std::uint32_t& writeOffset) override;
		virtual void Resize(const std::uint32_t& size) override;
	private:
		std::uint32_t	m_RenderID;
		Layout			m_Layout;
	};
}