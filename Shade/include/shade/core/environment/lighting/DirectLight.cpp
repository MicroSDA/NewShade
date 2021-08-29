#include "shade_pch.h"
#include "DirectLight.h"

std::uint32_t shade::DirectLight::m_TotalCount = 0;

shade::DirectLight::DirectLight() : shade::Light(shade::Environment::Type::DirectLight),
	m_Direction(0.0f, 0.0f, 1.0f)
{
	// Icnrease count
	m_TotalCount++;
}

shade::DirectLight::~DirectLight()
{
	// Decrease light count
	m_TotalCount--;
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

shade::DirectLight::RenderData shade::DirectLight::GetRenderData()
{
	return RenderData{glm::normalize(m_Direction), m_AmbientColor, m_DiffuseColor, m_SpecularColor};
}

std::uint32_t shade::DirectLight::GetTotalCount()
{
	return m_TotalCount;
}
