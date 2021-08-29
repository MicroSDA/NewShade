#pragma once
#include "shade/config/API.h"
#include "shade/core/Types.h"
#include "shade/core/vertex/Vertex3D.h"

namespace shade
{
	class SHADE_API IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;

		virtual void SetData(const Index* data, const uint32_t& count, const uint32_t& offset) = 0;
		virtual void Bind()   const = 0;
		virtual void Unbind() const = 0;
		virtual std::uint32_t GetSize()  const = 0;
		virtual std::uint32_t GetCount() const = 0;

		static Shared<IndexBuffer> Create(const Index* data, const std::uint32_t& count);
		static Shared<IndexBuffer> Create(const std::uint32_t& count);
	};
}