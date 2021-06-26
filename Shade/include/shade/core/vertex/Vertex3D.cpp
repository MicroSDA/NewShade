#include "shade_pch.h"
#include "Vertex3D.h"

shade::Vertex3D::Vertex3D() :
	Position(glm::vec3(0, 0, 0)),
	UV_Coordinates(glm::vec2(0, 0)),
	Normal(glm::vec3(0, 0, 0)),
	Tangent(glm::vec3(0, 0, 0))
{
}
