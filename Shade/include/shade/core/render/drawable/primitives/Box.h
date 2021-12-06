#pragma once
#include "shade/core/render/drawable/Drawable.h"

namespace shade
{
	class SHADE_API Box : public Drawable
	{
	public:
		static Shared<Box> Create(const glm::vec3& minHalfExt = glm::vec3(-0.5f), const glm::vec3& maxHalfExt = glm::vec3(0.5f));
		virtual ~Box() = default;
	private:
		Box(const glm::vec3& minHalfExt, const glm::vec3& maxHalfEx);
	};
}