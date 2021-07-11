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


	/*return;
	for (int j = 0; j <= density; ++j) {
		for (int i = 0; i <= density; ++i) {
			float x = (float)i / (float)density;
			float y = 0;
			float z = (float)j / (float)density;

			Vertex3D vertex;
			vertex.Position.x = (x * width) - width / 2;
			vertex.Position.y = 0.0f;
			vertex.Position.z = (z * height) - height / 2;

			AddVertex(vertex);
		}
	}

	for (int j = 0; j < density; ++j ) {
		for (int i = 0; i < density; ++i) {

			int row1 = j * (density + 1);
			int row2 = (j + 1) * (density + 1);

			AddIndex(row1 + i); AddIndex(row1 + i + 1); AddIndex(row1 + i + 1); AddIndex(row2 + i + 1);
			AddIndex(row2 + i + 1); AddIndex(row2 + i); AddIndex(row2 + i); AddIndex(row1 + i);
		}
	}*/

}
