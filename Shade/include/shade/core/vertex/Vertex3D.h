#pragma once
#include "shade/config/API.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace shade
{
	
	class SHADE_API Vertex3D
	{
	public:
		Vertex3D();
		glm::vec3 Position;
		glm::vec2 UV_Coordinates;
		glm::vec3 Normal;
		glm::vec3 Tangent;
	};

	typedef std::vector<Vertex3D>	Vertices;
	typedef std::uint32_t			Index;
	typedef std::vector<Index>		Indices;

}