#include "shade_pch.h"
#include "Light.h"

shade::Light::Light(const shade::Environment::Type& type) : shade::Environment(type),
	m_DiffuseColor(1.0f, 1.0f, 1.0f),
	m_SpecularColor(1.0f, 1.0f, 1.0f)
{
}


void shade::Light::SetDiffuseColor(const float& r, const float& g, const float& b)
{
	m_DiffuseColor = glm::vec3(r, g, b);
}

void shade::Light::SetSpecularColor(const float& r, const float& g, const float& b)
{
	m_SpecularColor = glm::vec3(r, g, b);
}

void shade::Light::SetDiffuseColor(const glm::vec3& color)
{
	m_DiffuseColor = color;
}

void shade::Light::SetSpecularColor(const glm::vec3& color)
{
	m_SpecularColor = color;
}


const glm::vec3& shade::Light::GetDiffuseColor() const
{
	return m_DiffuseColor;
}

const glm::vec3& shade::Light::GetSpecularColor() const
{
	return m_SpecularColor;
}

glm::vec3& shade::Light::GetDiffuseColor()
{
	return const_cast<glm::vec3&>(const_cast<const shade::Light*>(this)->GetDiffuseColor());
}

glm::vec3& shade::Light::GetSpecularColor()
{
	return const_cast<glm::vec3&>(const_cast<const shade::Light*>(this)->GetSpecularColor());
}
