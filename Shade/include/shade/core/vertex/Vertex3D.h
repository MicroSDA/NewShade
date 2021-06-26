#pragma once
#include "shade/config/API.h"
#include <glm/glm.hpp>

namespace shade
{
	typedef std::uint32_t Index;

	class SHADE_API Vertex3D
	{
	public:
		Vertex3D();
		glm::vec3 Position;
		glm::vec2 UV_Coordinates;
		glm::vec3 Normal;
		glm::vec3 Tangent;
	};
}