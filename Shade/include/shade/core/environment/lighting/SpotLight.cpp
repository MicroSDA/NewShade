#include "shade_pch.h"
#include "SpotLight.h"

std::uint32_t  shade::SpotLight::m_TotalCount = 0;

shade::SpotLight::SpotLight() : shade::Light(shade::Environment::Type::SpotLight),
	m_Distance(10.0f),
	m_Falloff(0.25f),
	m_MinAngle(glm::cos(glm::radians(5.5f))),
	m_MaxAngle(glm::cos(glm::radians(7.5f)))
{
	// Icnrease count
	m_TotalCount++;
}

shade::SpotLight::~SpotLight()
{
	// Decrease light count
	m_TotalCount--;
}

 void shade::SpotLight::SetMinAngle(const float& angle)
{
	m_MinAngle = angle;
}

 void shade::SpotLight::SetMaxAngle(const float& angle)
{
	m_MaxAngle = angle;
}

 const float& shade::SpotLight::GetMinAngle() const
{
	return m_MinAngle;
}

 float& shade::SpotLight::GetMinAngle()
 {
	 return const_cast<float&>(const_cast<const shade::SpotLight*>(this)->GetMinAngle());
 }

 const float& shade::SpotLight::GetMaxAngle() const
 {
	 return m_MaxAngle;
 }

 float& shade::SpotLight::GetMaxAngle()
 {
	 return const_cast<float&>(const_cast<const shade::SpotLight*>(this)->GetMaxAngle());
 }


 void shade::SpotLight::SetDistance(const float& distance)
 {
	 m_Distance = distance;
 }

 void shade::SpotLight::SetFalloff(const float& falloff)
 {
	 m_Falloff = falloff;
 }


 const float& shade::SpotLight::GetDistance() const
 {
	 return m_Distance;
 }

 float& shade::SpotLight::GetDistance()
 {
	 return const_cast<float&>(const_cast<const shade::SpotLight*>(this)->GetDistance());
 }

const float& shade::SpotLight::GetFalloff() const
{
	return m_Falloff;
}

float& shade::SpotLight::GetFalloff()
{
	return const_cast<float&>(const_cast<const shade::SpotLight*>(this)->GetFalloff());
}

shade::SpotLight::RenderData shade::SpotLight::GetRenderData(const glm::vec3& position, const glm::vec3& derection) const
{
	return RenderData { 
		position,
		glm::normalize(derection),
		m_DiffuseColor,
		m_SpecularColor,
		m_Intensity,
		m_Distance,
		m_Falloff,
		m_MinAngle,
		m_MaxAngle
	};
}

std::uint32_t shade::SpotLight::GetTotalCount()
{
	return m_TotalCount;
}
