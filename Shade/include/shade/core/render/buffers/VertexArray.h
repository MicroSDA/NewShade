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

		virtual void Bind()   = 0;
		virtual void UnBind() = 0;

		virtual void AddVertexBuffer(const Shared<VertexBuffer>& vertexBuffer) = 0;
		virtual void SetIndexBuffer(const  Shared<IndexBuffer>& indexBuffer) = 0;

		virtual const std::vector<Shared<VertexBuffer>>& GetVertexBuffers() const = 0;
		virtual const Shared<IndexBuffer>& GetIndexBuffer() const = 0;

		static Shared<VertexArray> Create();
	};
}