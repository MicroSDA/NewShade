#include "shade_pch.h"
#include "Grid.h"

shade::Shared<shade::Grid> shade::Grid::Create(const float& width, const float& height, const std::uint32_t& density)
{
	// Cannot accsses to private memmber, so without CreateShared
    return Shared<Grid>(new Grid(width, height, density));
}

shade::Grid::Grid(const float& width, const float& height, const std::uint32_t& density)
{
	SetDrawMode(Drawable::DrawMode::TriangleStrip);

	Vertex3D vertex;
	vertex.Position = glm::vec3(-1.0, 1.0, 0);
	AddVertex(vertex);
	vertex.Position = glm::vec3(-1.0, -1.0, 0);
	AddVertex(vertex);
	vertex.Position = glm::vec3(1.0, 1.0, 0);
	AddVertex(vertex);
	vertex.Position = glm::vec3(1.0, -1.0, 0);
	AddVertex(vertex);

	AddIndex(0);
	AddIndex(1);
	AddIndex(2);
	AddIndex(3);
}
