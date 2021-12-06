#include "shade_pch.h"
#include "Plane.h"

shade::Shared<shade::Plane> shade::Plane::Create(const std::uint32_t& width, const std::uint32_t& height)
{
    return Shared<Plane>(new Plane(width, height));
}

shade::Plane::Plane(const std::uint32_t& width, const std::uint32_t& height)
{
	SetDrawMode(Drawable::DrawMode::TriangleStrip);

	Vertex3D vertex;
	vertex.Position = glm::vec3(-1.0, 1.0, 0);
	vertex.UV_Coordinates = glm::fvec2(0.0, 0.0);
	AddVertex(vertex);
	vertex.Position = glm::vec3(-1.0, -1.0, 0);
	vertex.UV_Coordinates = glm::fvec2(0.0, 1.0);
	AddVertex(vertex);
	vertex.Position = glm::vec3(1.0, 1.0, 0);
	vertex.UV_Coordinates = glm::fvec2(1.0, 0.0);
	AddVertex(vertex);
	vertex.Position = glm::vec3(1.0, -1.0, 0);
	vertex.UV_Coordinates = glm::fvec2(1.0, 1.0);
	AddVertex(vertex);

	AddIndex(0);
	AddIndex(1);
	AddIndex(2);
	AddIndex(3);
}
