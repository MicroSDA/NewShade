#pragma once
#include "shade/core/render/drawable/Drawable.h"

namespace shade
{
	class SHADE_API Box : public Drawable
	{
	public:
		static Shared<Box> Create(const std::uint32_t& width, const std::uint32_t& height);
		virtual ~Box() = default;
	private:
		Box(const std::uint32_t& width, const std::uint32_t& height);
	};
}