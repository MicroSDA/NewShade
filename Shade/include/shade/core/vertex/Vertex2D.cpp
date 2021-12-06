#include "shade_pch.h"
#include "Vertex2D.h"

shade::Vertex2D::Vertex2D() :
	Position(glm::vec2(0, 0)),
	UV_Coordinates(glm::vec2(0, 0))
{
}

shade::Vertex2D::Vertex2D(const glm::vec2& position, const glm::vec2& uv) :
	Position(position), UV_Coordinates(uv)
{
}
