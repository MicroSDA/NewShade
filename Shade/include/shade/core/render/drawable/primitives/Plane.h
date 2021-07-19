#pragma once
#include "shade/core/render/drawable/Drawable.h"

namespace shade
{
	class SHADE_API Plane : public Drawable
	{
	public:
		static Shared<Plane> Create(const std::uint32_t& width, const std::uint32_t& height);
		virtual ~Plane() = default;
	private:
		Plane(const std::uint32_t& width, const std::uint32_t& height);
	};
}