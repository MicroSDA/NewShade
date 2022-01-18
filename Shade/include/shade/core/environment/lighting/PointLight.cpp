#include "shade_pch.h"
#include "PointLight.h"

std::uint32_t  shade::PointLight::m_TotalCount = 0;

shade::PointLight::PointLight() : shade::Light(shade::Environment::Type::PointLight),
	m_Distance(10.0f),
	m_Falloff(0.25f)
{
	// Icnrease count
	m_TotalCount++;
}

shade::PointLight::~PointLight()
{
	// Decrease light count
	m_TotalCount--;
}

 void shade::PointLight::SetDistance(const float& distnace)
{
	m_Distance = distnace;
}

 void shade::PointLight::SetFalloff(const float& falloff)
{
	m_Falloff = falloff;
}

 const float& shade::PointLight::GetDistance() const
{
	return  m_Distance;
}

float& shade::PointLight::GetDistance()
{
	return const_cast<float&>(const_cast<const shade::PointLight*>(this)->GetDistance());
}

 const float& shade::PointLight::GetFalloff() const
{
	return m_Falloff;
}

float& shade::PointLight::GetFalloff()
{
	return const_cast<float&>(const_cast<const shade::PointLight*>(this)->GetFalloff());
}

shade::PointLight::RenderData shade::PointLight::GetRenderData(const glm::vec3& position)
{
	return RenderData{ position, m_DiffuseColor, m_SpecularColor, m_Intensity, m_Distance, m_Falloff};
}

std::uint32_t shade::PointLight::GetTotalCount()
{
	return m_TotalCount;
}
