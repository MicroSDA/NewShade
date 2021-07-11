#pragma once
#include "shade/config/API.h"
#include "shade/core/render/buffers/VertexBuffer.h"
#include "shade/core/render/buffers/IndexBuffer.h"

namespace shade
{
	class SHADE_API VertexArray
	{
	public:
		virtual ~VertexArray() = default;

		virtual void Bind()   const = 0;
		virtual void UnBind() const = 0;

		virtual void AddVertexBuffer(const Shared<VertexBuffer>& vertexBuffer) = 0;
		virtual void SetIndexBuffer(const  Shared<IndexBuffer>& indexBuffer) = 0;

		const std::vector<Shared<VertexBuffer>>& GetVertexBuffers() const;
		std::vector<Shared<VertexBuffer>>& GetVertexBuffers();
		const Shared<IndexBuffer>& GetIndexBuffer() const;
		Shared<IndexBuffer>& GetIndexBuffer();
		
		void RemoveVertexBuffer(const std::uint32_t& renderId);

		static Shared<VertexArray> Create();
	protected:
		std::vector<Shared<VertexBuffer>> m_VertexBuffers;
		Shared<IndexBuffer> m_IndexBuffer;
	};
}