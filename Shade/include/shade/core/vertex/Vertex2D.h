#pragma once
#include "shade/config/API.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace shade
{
	class SHADE_API Vertex2D
	{
	public:
		Vertex2D();
		Vertex2D(const glm::vec2& position, const glm::vec2& uv);
		glm::vec2 Position;
		glm::vec2 UV_Coordinates;
	};
}