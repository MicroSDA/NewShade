#include "shade_pch.h"
#include "DirectLight.h"

shade::DirectLight::DirectLight() : shade::Light(shade::Environment::Type::DirectLight),
	m_Direction(0.0f, 0.0f, 1.0f)
{
}

void shade::DirectLight::SetDirection(const float& x, const float& y, const float& z)
{
	m_Direction = glm::vec3(x, y, z);
}

void shade::DirectLight::SetDirection(const glm::fvec3& direction)
{
	m_Direction = direction;
}

const glm::fvec3& shade::DirectLight::GetDirection() const
{
	return m_Direction;
}

glm::fvec3& shade::DirectLight::GetDirection()
{
	return const_cast<glm::fvec3&>(const_cast<const shade::DirectLight*>(this)->GetDirection());
}

void shade::DirectLight::Process(const Shared<Shader>& shader)
{
	shader->SendFlaot3("u_Lighting._DirectLight.Direction", glm::value_ptr(glm::normalize(m_Direction)));
	shader->SendFlaot3("u_Lighting._DirectLight.AmbientColor", glm::value_ptr(m_AmbientColor));
	shader->SendFlaot3("u_Lighting._DirectLight.DiffuseColor", glm::value_ptr(m_DiffuseColor));
	shader->SendFlaot3("u_Lighting._DirectLight.SpecularColor", glm::value_ptr(m_SpecularColor));
}
