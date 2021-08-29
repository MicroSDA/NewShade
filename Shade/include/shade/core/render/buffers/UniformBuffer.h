#pragma once
#include "shade/config/API.h"
#include "shade/core/Types.h"

namespace shade
{
	class SHADE_API UniformBuffer
	{
	public:
		virtual ~UniformBuffer() = default;
		virtual void SetData(const void* data, const std::uint32_t& size, const std::uint32_t& offset = 0) = 0;
		virtual std::uint32_t GetSize() = 0;
		static Shared<UniformBuffer> Create(const std::uint32_t& size, const std::uint32_t& bindng);
	};
}

