#pragma once
#include "shade/config/API.h"
#include "shade/core/Types.h"

namespace shade
{
	class SHADE_API ShaderStorageBuffer
	{
	public:
		virtual ~ShaderStorageBuffer() = default;
		virtual void SetData(const void* data, const std::uint32_t & size, const std::uint32_t & offset = 0) = 0;

		static Shared<ShaderStorageBuffer> Create(const std::uint32_t & size, const std::uint32_t & bindng);
	};

}