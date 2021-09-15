#pragma once
#include "shade/core/render/buffers/VertexArray.h"

namespace shade
{
	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();

		virtual void Bind()   const override;
		virtual void UnBind() const override;

		virtual void AddVertexBuffer(const Shared<VertexBuffer>& vertexBuffer) override;
		virtual void SetIndexBuffer(const Shared<IndexBuffer>& indexBuffer) override;

		virtual const std::vector<Shared<VertexBuffer>>& GetVertexBuffers() const;
		virtual std::vector<Shared<VertexBuffer>>& GetVertexBuffers();
		virtual const Shared<IndexBuffer>& GetIndexBuffer() const;

		virtual const std::uint32_t& GetRenderID()  const;
	private:
		uint32_t m_RenderID;
		uint32_t m_VertexBufferIndex = 0;
	};
}