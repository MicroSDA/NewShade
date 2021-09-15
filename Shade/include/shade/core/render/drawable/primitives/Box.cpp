#include "shade_pch.h"
#include "Box.h"

shade::Shared<shade::Box> shade::Box::Create(const glm::vec3& minHalfExt, const glm::vec3& maxHalfExt)
{
	return Shared<shade::Box>(new Box(minHalfExt, maxHalfExt));
}

shade::Box::Box(const glm::vec3& minHalfExt, const glm::vec3& maxHalfExt)
{
  /*points[0] = clipSpaceMatrix * glm::vec4(minHalfExt.x, maxHalfExt.y, minHalfExt.z, 1.f);1
	points[1] = clipSpaceMatrix * glm::vec4(minHalfExt.x, maxHalfExt.y, maxHalfExt.z, 1.f);2
	points[2] = clipSpaceMatrix * glm::vec4(maxHalfExt.x, maxHalfExt.y, maxHalfExt.z, 1.f);3
	points[3] = clipSpaceMatrix * glm::vec4(maxHalfExt.x, maxHalfExt.y, minHalfExt.z, 1.f);4
	points[4] = clipSpaceMatrix * glm::vec4(maxHalfExt.x, minHalfExt.y, minHalfExt.z, 1.f);5
	points[5] = clipSpaceMatrix * glm::vec4(maxHalfExt.x, minHalfExt.y, maxHalfExt.z, 1.f);6
	points[6] = clipSpaceMatrix * glm::vec4(minHalfExt.x, minHalfExt.y, maxHalfExt.z, 1.f);7
	points[7] = clipSpaceMatrix * glm::vec4(minHalfExt.x, minHalfExt.y, minHalfExt.z, 1.f);8
  */
	SetDrawMode(Drawable::DrawMode::Lines);

	Vertex3D vertex;
	vertex.Position = glm::vec3(minHalfExt.x, minHalfExt.y, minHalfExt.z); 
	AddVertex(vertex);
	vertex.Position = glm::vec3(maxHalfExt.x, minHalfExt.y, minHalfExt.z); 
	AddVertex(vertex);
	vertex.Position = glm::vec3(maxHalfExt.x, maxHalfExt.y, minHalfExt.z); 
	AddVertex(vertex);
	vertex.Position = glm::vec3(minHalfExt.x, maxHalfExt.y, minHalfExt.z); 
	AddVertex(vertex);
	vertex.Position = glm::vec3(minHalfExt.x, minHalfExt.y, maxHalfExt.z); 
	AddVertex(vertex);
	vertex.Position = glm::vec3(maxHalfExt.x, minHalfExt.y, maxHalfExt.z);
	AddVertex(vertex);
	vertex.Position = glm::vec3(maxHalfExt.x, maxHalfExt.y, maxHalfExt.z);
	AddVertex(vertex);
	vertex.Position = glm::vec3(minHalfExt.x, maxHalfExt.y, maxHalfExt.z);
	AddVertex(vertex);

	AddIndex(0); AddIndex(1); AddIndex(1); AddIndex(2); AddIndex(2); AddIndex(3); AddIndex(3); AddIndex(0);
	AddIndex(4); AddIndex(5); AddIndex(5); AddIndex(6); AddIndex(6); AddIndex(7); AddIndex(7); AddIndex(4);
	AddIndex(0); AddIndex(4); AddIndex(1); AddIndex(5); AddIndex(2); AddIndex(6); AddIndex(3); AddIndex(7);


	/*Vertex3D vertex;
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
	*/
}
