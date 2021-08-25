#include "shade_pch.h"
#include "Box.h"

shade::Shared<shade::Box> shade::Box::Create(const std::uint32_t& width, const std::uint32_t& height)
{
	return Shared<shade::Box>(new Box(width, height));
}

shade::Box::Box(const std::uint32_t& width, const std::uint32_t& height)
{
	SetDrawMode(Drawable::DrawMode::Points);

	
	Vertex3D vertex;
	vertex.Position = glm::vec3(-1, -1, -1);
	AddVertex(vertex);
	vertex.Position = glm::vec3(1, -1, -1);
	AddVertex(vertex);
	vertex.Position = glm::vec3(1, 1, -1);
	AddVertex(vertex);
	vertex.Position = glm::vec3(-1, 1, -1);
	AddVertex(vertex);
	
	vertex.Position = glm::vec3(-1, -1, 1);
	AddVertex(vertex);
	vertex.Position = glm::vec3(1, -1, 1);
	AddVertex(vertex);
	vertex.Position = glm::vec3(1, 1, 1);
	AddVertex(vertex);
	vertex.Position = glm::vec3(-1, 1, 1);
	AddVertex(vertex);

	
	AddIndex(0); AddIndex(1); AddIndex(1); AddIndex(2); AddIndex(2); AddIndex(3); AddIndex(3); AddIndex(0);
	AddIndex(4); AddIndex(5); AddIndex(5); AddIndex(6); AddIndex(6); AddIndex(7); AddIndex(7); AddIndex(4);
	AddIndex(0); AddIndex(4); AddIndex(1); AddIndex(5); AddIndex(2); AddIndex(6); AddIndex(3); AddIndex(7);
	
}
