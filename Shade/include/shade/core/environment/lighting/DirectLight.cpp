#include "shade_pch.h"
#include "DirectLight.h"

std::uint32_t shade::DirectLight::m_TotalCount = 0;

shade::DirectLight::DirectLight() : shade::Light(shade::Environment::Type::DirectLight)
{
	// Icnrease count
	m_TotalCount++;
}

shade::DirectLight::~DirectLight()
{
	// Decrease light count
	m_TotalCount--;
}

shade::DirectLight::RenderData shade::DirectLight::GetRenderData(const glm::vec3& direction)
{
	return RenderData{ glm::normalize(direction), m_AmbientColor, m_DiffuseColor, m_SpecularColor};
}

std::uint32_t shade::DirectLight::GetTotalCount()
{
	return m_TotalCount;
}
