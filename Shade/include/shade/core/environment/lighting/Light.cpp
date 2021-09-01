#include "shade_pch.h"
#include "Light.h"

shade::Light::Light(const shade::Environment::Type& type) : shade::Environment(type),
	m_AmbientColor(0.1f, 0.1f, 0.1f),
	m_DiffuseColor(1.0f, 1.0f, 1.0f),
	m_SpecularColor(1.0f, 1.0f, 1.0f)
{
}

void shade::Light::SetAmbientColor(const float& r, const float& g, const float& b)
{
	m_AmbientColor = glm::vec3(r, g, b);
}

void shade::Light::SetDiffuseColor(const float& r, const float& g, const float& b)
{
	m_DiffuseColor = glm::vec3(r, g, b);
}

void shade::Light::SetSpecularColor(const float& r, const float& g, const float& b)
{
	m_SpecularColor = glm::vec3(r, g, b);
}

void shade::Light::SetAmbientColor(const glm::vec3& color)
{
	m_AmbientColor = color;
}

void shade::Light::SetDiffuseColor(const glm::vec3& color)
{
	m_DiffuseColor = color;
}

void shade::Light::SetSpecularColor(const glm::vec3& color)
{
	m_SpecularColor = color;
}

const glm::vec3& shade::Light::GetAmbientColor() const
{
	return m_AmbientColor;
}

const glm::vec3& shade::Light::GetDiffuseColor() const
{
	return m_DiffuseColor;
}

const glm::vec3& shade::Light::GetSpecularColor() const
{
	return m_SpecularColor;
}

glm::vec3& shade::Light::GetAmbientColor()
{
	return const_cast<glm::vec3&>(const_cast<const shade::Light*>(this)->GetAmbientColor());
}

glm::vec3& shade::Light::GetDiffuseColor()
{
	return const_cast<glm::vec3&>(const_cast<const shade::Light*>(this)->GetDiffuseColor());
}

glm::vec3& shade::Light::GetSpecularColor()
{
	return const_cast<glm::vec3&>(const_cast<const shade::Light*>(this)->GetSpecularColor());
}
