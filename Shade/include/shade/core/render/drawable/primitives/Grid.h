#pragma once
#include "shade/config/API.h"
#include "shade/core/render/drawable/Drawable.h"

namespace shade
{
	class SHADE_API Grid : public Drawable
	{
	public:
		static Shared<Grid> Create(const float& width, const float& height, const std::uint32_t& density);
		virtual ~Grid() = default;
	private:
		Grid(const float& width, const float& height, const std::uint32_t& density);
		glm::vec2 m_Size;
		std::uint32_t m_Density;
	};
}