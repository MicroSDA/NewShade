#pragma once
#include "shade/core/render/buffers/VertexArray.h"

namespace shade
{
	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();

		virtual void Bind() override;
		virtual void UnBind() override;

		virtual void AddVertexBuffer(const Shared<VertexBuffer>& vertexBuffer) override;
		virtual void SetIndexBuffer(const Shared<IndexBuffer>& indexBuffer) override;

		virtual const std::vector<Shared<VertexBuffer>>& GetVertexBuffers() const;
		virtual const Shared<IndexBuffer>& GetIndexBuffer() const;
	private:
		uint32_t m_RenderID;
		uint32_t m_VertexBufferIndex = 0;
		std::vector<Shared<VertexBuffer>> m_VertexBuffers;
		Shared<IndexBuffer> m_IndexBuffer;
	};
}