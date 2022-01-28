#include "shade_pch.h"
#include "Sphere.h"

shade::Shared<shade::Sphere> shade::Sphere::Create(const float& radius, const float& density, const float& step)
{
	return Shared<shade::Sphere>(new Sphere(radius, density, step));
}

shade::Sphere::Sphere(const float& radius, const float& density, const float& step)
{
	SetDrawMode(Drawable::DrawMode::Lines);

	/* Top */
	//AddVertex(glm::vec3(0.0f, radius, 0.0f));  AddIndex(0);

	const float _step = std::atan(1.0f) * step / density;

	for (int i = 0; i < density; i++) {
		float a = i * _step;
		AddVertex(glm::vec3(std::cos(a) * radius, std::sin(a) * radius, 0.0f));
		AddIndex(i);
	}
	for (int i = 0; i < density; i++) {
		float a = i * _step;
		AddVertex(glm::vec3(0.0f, std::cos(a) * radius, std::sin(a) * radius));
		AddIndex(density + i);
	}
	for (int i = 0; i < density; i++) {
		float a = i * _step;
		AddVertex(glm::vec3(std::cos(a) * radius, 0.0f, std::sin(a) * radius));
		AddIndex(density + density + i);
	}
	/* Bottom */
	//AddVertex(glm::vec3(0.0f, -radius, 0.0f)); AddIndex(wDensity + hDensity + 2);
	///* Left */
	//AddVertex(glm::vec3(radius, 0.0f, 0.0f));  AddIndex(2);
	///* Right */
	//AddVertex(glm::vec3(-radius, 0.0f, 0.0f)); AddIndex(3);
	///* Forward */
	//AddVertex(glm::vec3(0.0f, 0.0f, radius));  AddIndex(4);
	///* Backword */
	//AddVertex(glm::vec3(0.0f, 0.0f, -radius)); AddIndex(5);
}