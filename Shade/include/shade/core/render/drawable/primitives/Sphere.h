#pragma once
#include "shade/core/render/drawable/Drawable.h"

namespace shade
{
	class SHADE_API Sphere : public Drawable
	{
	public:
		static Shared<Sphere> Create(const float& radius, const float& wDensity, const float& hDensity);
		virtual ~Sphere() = default;
	private:
		Sphere(const float& radius, const float& wDensity, const float& hDensity);
	};
}